/**
 * @file CloudOTA.cpp
 * @brief Over-the-air firmware update driven by the "ota" cloud variable.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CloudOTA.hpp"

#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_STANDALONE)

#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_system.h"
#include "cJSON.h"

static const char* TAG = "CloudOTA";

// Streaming buffer: the image is never staged in RAM, this single chunk is reused
// for every esp_http_client_read() -> esp_ota_write() hop.
static const int BUFFER_SIZE = 1024;
static char otaWriteData[BUFFER_SIZE + 1] = {0};

// Timing / sizing
static const int PROGRESS_STEP_BYTES = 64 * 1024;  // publish PROGRESS every 64 KB
static const uint32_t OTA_TASK_STACK = 8192;       // the TLS handshake needs the room
static const UBaseType_t OTA_TASK_PRIO = 5;
static const int HTTP_TIMEOUT_MS = 10000;
static const uint32_t END_FLUSH_DELAY_MS = 1000;   // let END leave the client before rebooting
static const int MAX_REDIRECTS = 5;

static const char* HTTPS_PREFIX = "https://";

CloudOTA* CloudOTA::_instance = nullptr;

CloudOTA::CloudOTA()
    : _publish(nullptr)
    , _inProgress(false)
    , _taskHandle(nullptr)
{}

CloudOTA* CloudOTA::getInstance(void) {
    if (_instance == nullptr) {
        _instance = new CloudOTA();
    }
    return _instance;
}

void CloudOTA::setPublisher(std::function<void(const std::string&)> fn) {
    _publish = fn;
}

void CloudOTA::setHost(const std::string& host) {
    _host = host;
}

void CloudOTA::setToken(const std::string& token) {
    _token = token;
}

bool CloudOTA::isInProgress(void) const {
    return _inProgress;
}

void CloudOTA::_send(CloudOtaCmd cmd, int args) {
    if (!_publish) {
        ESP_LOGW(TAG, "No publisher installed, dropping ota message (cmd %d, args %d)",
                 (int)cmd, args);
        return;
    }

    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "cmd", (int)cmd);
    cJSON_AddNumberToObject(root, "args", args);
    char* str = cJSON_PrintUnformatted(root);
    if (str) {
        _publish(std::string(str));
        cJSON_free(str);
    }
    cJSON_Delete(root);
}

/* ------------------------------------------------------------------------- */
/* Inbound command handling                                                  */
/* ------------------------------------------------------------------------- */

void CloudOTA::handlePayload(const std::string& payload) {
    // Runs on the MQTT event task: parse and hand off, never block.
    cJSON* root = cJSON_Parse(payload.c_str());
    if (!root) {
        ESP_LOGW(TAG, "ota payload is not valid JSON: %s", payload.c_str());
        return;
    }

    cJSON* cmd = cJSON_GetObjectItem(root, "cmd");
    if (!cJSON_IsNumber(cmd)) {
        ESP_LOGW(TAG, "ota payload has no numeric \"cmd\": %s", payload.c_str());
        cJSON_Delete(root);
        return;
    }

    // Only UPDATE is inbound. PROGRESS and END are our own messages echoed back by
    // the broker on the topic we both publish to and subscribe to, so ignore them.
    if (cmd->valueint != (int)CloudOtaCmd::UPDATE) {
        cJSON_Delete(root);
        return;
    }

    if (_inProgress) {
        ESP_LOGW(TAG, "Update already in progress, ignoring UPDATE command");
        cJSON_Delete(root);
        return;
    }

    cJSON* args = cJSON_GetObjectItem(root, "args");
    if (!cJSON_IsString(args) || args->valuestring == nullptr ||
        args->valuestring[0] == '\0') {
        ESP_LOGE(TAG, "UPDATE command without a firmware URL in \"args\"");
        cJSON_Delete(root);
        _send(CloudOtaCmd::END, ESP_ERR_INVALID_ARG);
        return;
    }

    std::string url = args->valuestring;
    cJSON_Delete(root);

    // The cloud sends a path, not an absolute URL: resolve it against the host.
    if (url.compare(0, strlen(HTTPS_PREFIX), HTTPS_PREFIX) != 0) {
        // Every other network path of the Cloud library is TLS-verified; an
        // unauthenticated firmware fetch over plain HTTP is refused.
        if (url.compare(0, 5, "http:") == 0) {
            ESP_LOGE(TAG, "Firmware URL must be https:// (got %s)", url.c_str());
            _send(CloudOtaCmd::END, ESP_ERR_INVALID_ARG);
            return;
        }
        if (_host.empty()) {
            ESP_LOGE(TAG, "No cloud host set, cannot resolve firmware path %s", url.c_str());
            _send(CloudOtaCmd::END, ESP_ERR_INVALID_STATE);
            return;
        }
        if (url[0] != '/') {
            url.insert(0, 1, '/');
        }
        url.insert(0, std::string(HTTPS_PREFIX) + _host);
    }

    _url = url;
    _inProgress = true;

    BaseType_t ret = xTaskCreate(_task, "Cloud OTA", OTA_TASK_STACK, this,
                                 OTA_TASK_PRIO, &_taskHandle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create the OTA task");
        _taskHandle = nullptr;
        _inProgress = false;
        _send(CloudOtaCmd::END, ESP_ERR_NO_MEM);
        return;
    }
    ESP_LOGI(TAG, "OTA started from %s", _url.c_str());
}

/* ------------------------------------------------------------------------- */
/* Download / flash task                                                     */
/* ------------------------------------------------------------------------- */

void CloudOTA::_task(void* arg) {
    CloudOTA* self = static_cast<CloudOTA*>(arg);
    self->_run();
    // _run() only returns on failure; on success it reboots the module.
    self->_taskHandle = nullptr;
    self->_inProgress = false;
    vTaskDelete(nullptr);
}

void CloudOTA::_run(void) {
    esp_err_t err = ESP_OK;
    esp_ota_handle_t updateHandle = 0;
    bool otaBegun = false;
    esp_http_client_handle_t client = nullptr;
    int contentLength = 0;
    int redirects = 0;
    int written = 0;
    int lastReported = 0;
    bool headerChecked = false;
    std::string authHeader;
    bool authSent = false;

    const esp_partition_t* running = esp_ota_get_running_partition();
    const esp_partition_t* updatePartition = esp_ota_get_next_update_partition(NULL);
    if (updatePartition == NULL) {
        ESP_LOGE(TAG, "No OTA partition available");
        _send(CloudOtaCmd::END, ESP_ERR_NOT_FOUND);
        return;
    }
    ESP_LOGI(TAG, "Running subtype %d at offset 0x%08x, writing subtype %d at offset 0x%08x",
             running->subtype, (unsigned int)running->address,
             updatePartition->subtype, (unsigned int)updatePartition->address);

    esp_http_client_config_t config = {};
    config.url = _url.c_str();
    config.crt_bundle_attach = esp_crt_bundle_attach;
    config.timeout_ms = HTTP_TIMEOUT_MS;
    config.keep_alive_enable = true;

    client = esp_http_client_init(&config);
    if (client == nullptr) {
        ESP_LOGE(TAG, "Failed to init the HTTP client");
        _send(CloudOtaCmd::END, ESP_ERR_NO_MEM);
        return;
    }

    // The firmware endpoint is authenticated like the rest of the REST API
    // (see CloudProvisioning::performRequest).
    if (!_token.empty()) {
        authHeader = "Bearer " + _token;
        esp_http_client_set_header(client, "Authorization", authHeader.c_str());
        authSent = true;
    }

    // Open the connection, following redirections: firmware links are commonly
    // served through one (object storage, release assets...).
    while (true) {
        err = esp_http_client_open(client, 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open the HTTP connection: %s", esp_err_to_name(err));
            goto fail;
        }
        contentLength = esp_http_client_fetch_headers(client);
        int status = esp_http_client_get_status_code(client);

        if (status == 301 || status == 302 || status == 307 || status == 308) {
            if (++redirects > MAX_REDIRECTS) {
                ESP_LOGE(TAG, "Too many redirections");
                err = ESP_ERR_INVALID_RESPONSE;
                goto fail;
            }
            err = esp_http_client_set_redirection(client);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to follow the redirection: %s", esp_err_to_name(err));
                goto fail;
            }
            // Firmware links commonly redirect to an object storage bucket: drop the
            // device token rather than handing it to a third-party host.
            if (authSent) {
                esp_http_client_delete_header(client, "Authorization");
                authSent = false;
            }
            // Drain the redirection body so the connection can be reused.
            while (esp_http_client_read(client, otaWriteData, BUFFER_SIZE) > 0) {
                ;
            }
            continue;
        }

        if (status != 200) {
            ESP_LOGE(TAG, "Firmware download failed with HTTP status %d", status);
            err = ESP_ERR_INVALID_RESPONSE;
            goto fail;
        }
        break;
    }

    if (contentLength > 0 && (size_t)contentLength > updatePartition->size) {
        ESP_LOGE(TAG, "Firmware is %d bytes, larger than the %u byte partition",
                 contentLength, (unsigned int)updatePartition->size);
        err = ESP_ERR_INVALID_SIZE;
        goto fail;
    }

    while (true) {
        int dataRead = esp_http_client_read(client, otaWriteData, BUFFER_SIZE);

        if (dataRead < 0) {
            ESP_LOGE(TAG, "SSL data read error");
            err = ESP_FAIL;
            goto fail;
        } else if (dataRead > 0) {
            if (!headerChecked) {
                // The app descriptor sits right after the image and first segment
                // headers; log the incoming version before flashing anything.
                size_t descOffset = sizeof(esp_image_header_t) +
                                    sizeof(esp_image_segment_header_t);
                if ((size_t)dataRead <= descOffset + sizeof(esp_app_desc_t)) {
                    ESP_LOGE(TAG, "First chunk too small to hold the image header");
                    err = ESP_ERR_INVALID_SIZE;
                    goto fail;
                }
                esp_app_desc_t newAppInfo;
                memcpy(&newAppInfo, &otaWriteData[descOffset], sizeof(esp_app_desc_t));
                ESP_LOGI(TAG, "New firmware version: %s", newAppInfo.version);

                headerChecked = true;
                err = esp_ota_begin(updatePartition, OTA_WITH_SEQUENTIAL_WRITES,
                                    &updateHandle);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));
                    goto fail;
                }
                otaBegun = true;
            }

            err = esp_ota_write(updateHandle, (const void*)otaWriteData, dataRead);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "esp_ota_write failed (%s)", esp_err_to_name(err));
                goto fail;
            }
            written += dataRead;

            if ((written - lastReported) >= PROGRESS_STEP_BYTES) {
                _send(CloudOtaCmd::PROGRESS, written);
                lastReported = written;
            }
        } else { // dataRead == 0
            if (errno == ECONNRESET || errno == ENOTCONN) {
                ESP_LOGE(TAG, "Connection closed, errno = %d", errno);
                err = ESP_FAIL;
                goto fail;
            }
            if (esp_http_client_is_complete_data_received(client)) {
                break;
            }
        }
    }

    ESP_LOGI(TAG, "Total written binary data length: %d", written);

    if (!esp_http_client_is_complete_data_received(client)) {
        ESP_LOGE(TAG, "Incomplete firmware received");
        err = ESP_ERR_INVALID_STATE;
        goto fail;
    }
    if (!otaBegun) {
        ESP_LOGE(TAG, "Empty firmware received");
        err = ESP_ERR_INVALID_SIZE;
        goto fail;
    }

    err = esp_ota_end(updateHandle);
    otaBegun = false; // the handle is consumed, it must not be aborted
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(TAG, "Image validation failed, image is corrupted");
        } else {
            ESP_LOGE(TAG, "esp_ota_end failed (%s)", esp_err_to_name(err));
        }
        goto fail;
    }

    err = esp_ota_set_boot_partition(updatePartition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed (%s)", esp_err_to_name(err));
        goto fail;
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    _send(CloudOtaCmd::PROGRESS, written);
    _send(CloudOtaCmd::END, ESP_OK);

    // Give the MQTT client time to actually flush END before the reboot.
    ESP_LOGI(TAG, "OTA succeeded, restarting on the new partition");
    vTaskDelay(pdMS_TO_TICKS(END_FLUSH_DELAY_MS));
    esp_restart();
    return;

fail:
    // Abort, report the error code and keep running the current firmware: a new
    // UPDATE command can start a fresh attempt.
    if (otaBegun) {
        esp_ota_abort(updateHandle);
    }
    if (client) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
    }
    ESP_LOGE(TAG, "OTA failed: %s", esp_err_to_name(err));
    _send(CloudOtaCmd::END, (int)err);
}

#endif // CONFIG_MODULE_MASTER || CONFIG_MODULE_STANDALONE
