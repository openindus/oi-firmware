/**
 * @file Cloud.cpp
 * @brief Implementation file for the Cloud API component with MQTT support.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Cloud.hpp"
#include "MQTTManager.hpp"
#include "CloudProvisioning.hpp"
#include "Board.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "nvs.h"
#include "cJSON.h"

static const char* TAG = "Cloud";

static const char* NVS_NAMESPACE = "oi_cloud";
static const char* NVS_KEY_UUID = "dev_uuid";
static const char* NVS_KEY_TOKEN = "dev_token";
static const char* NVS_KEY_PLATFORM = "plat_uuid";

// Timing (ms)
static const uint32_t PROVISION_RETRY_MS = 5000;
static const uint32_t STATUS_POLL_MS = 5000;
static const uint32_t SERVICE_PERIOD_MS = 100;

Cloud::Cloud(const char* platformUuid, const char* platformToken, int projectId)
    : _projectId(projectId)
    , _haveCredentials(false)
    , _state(CloudState::IDLE)
    , _taskHandle(nullptr)
    , _varLog(nullptr)
    , _varVersion(nullptr)
    , _varStatus(nullptr)
    , _varRestart(nullptr)
    , _varOta(nullptr)
{
    if (platformUuid) {
        _platformUuid = platformUuid;
    }
    if (platformToken) {
        _platformToken = platformToken;
    }

    // Device name = system MAC address (used as provisioning "name")
    uint8_t mac[6];
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        char mac_str[18];
        snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        _deviceName = mac_str;
    } else {
        ESP_LOGE(TAG, "Failed to get system MAC address");
        _deviceName = "00:00:00:00:00:00";
    }

    ESP_LOGI(TAG, "Cloud instance created (device name: %s)", _deviceName.c_str());
}

Cloud::~Cloud() {
    end();
    ESP_LOGI(TAG, "Cloud instance destroyed");
}

void Cloud::useDeviceCredentials(const char* uuid, const char* token) {
    if (uuid && token) {
        _deviceUuid = uuid;
        _deviceToken = token;
        _haveCredentials = true;
        ESP_LOGI(TAG, "Using provided device credentials");
    }
}

bool Cloud::begin() {
    return begin("oicloud.openindus.com");
}

bool Cloud::begin(const char* host) {
    if (!host) {
        ESP_LOGE(TAG, "Host is required");
        return false;
    }
    if (_taskHandle) {
        ESP_LOGW(TAG, "Cloud already started");
        return true;
    }

    _host = host;
    _setupDefaultVariables();

    BaseType_t ret = xTaskCreate(_task, "Cloud task", 8192, this, 5, &_taskHandle);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Cloud task");
        _taskHandle = nullptr;
        return false;
    }
    ESP_LOGI(TAG, "Cloud task started (host: %s)", _host.c_str());
    return true;
}

void Cloud::end(void) {
    if (_taskHandle) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
    MQTTManager::getInstance()->disconnect();
    _state = CloudState::IDLE;

    // Free owned default variables and drop them from the serviced list
    for (auto* v : _defaultVariables) {
        _variables.erase(std::remove(_variables.begin(), _variables.end(), v), _variables.end());
        delete v;
    }
    _defaultVariables.clear();
    _varLog = _varVersion = _varOta = nullptr;
    _varStatus = nullptr;
    _varRestart = nullptr;
}

CloudState Cloud::getState(void) const {
    return _state;
}

bool Cloud::isConnected(void) const {
    return _state == CloudState::CONNECTED && MQTTManager::getInstance()->isConnected();
}

/* ------------------------------------------------------------------------- */
/* Variable registration                                                     */
/* ------------------------------------------------------------------------- */

void Cloud::_addVariable(ICloudVariable* var) {
    if (!var) {
        return;
    }
    _variables.push_back(var);
    ESP_LOGI(TAG, "Registered variable: %s", var->getName().c_str());
}

void Cloud::unregisterVariable(const std::string& name) {
    for (auto it = _variables.begin(); it != _variables.end(); ++it) {
        if ((*it)->getName() == name) {
            _variables.erase(it);
            ESP_LOGI(TAG, "Unregistered variable: %s", name.c_str());
            return;
        }
    }
}

std::string Cloud::_topicFor(const ICloudVariable* var) const {
    char prefix = var->getTypePrefix();
    std::string type = (prefix == 'd') ? "def" : std::string(1, prefix);
    return "device/" + _deviceUuid + "/" + type + "/" + var->getName();
}

void Cloud::_subscribeVariable(ICloudVariable* var) {
    if (!var->isSubscriber()) {
        return;
    }
    std::string topic = _topicFor(var);
    MQTTManager::getInstance()->subscribe(topic, [var](const std::string& payload) {
        var->applyPayload(payload);
    });
}

/* ------------------------------------------------------------------------- */
/* Default variables (spec 2.2)                                              */
/* ------------------------------------------------------------------------- */

void Cloud::_setupDefaultVariables(void) {
    if (!_defaultVariables.empty()) {
        return; // already set up
    }

    _varLog = new StringVariable("log", "", UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH);
    _varVersion = new StringVariable("version", "", UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH);
    _varStatus = new IntVariable("status", 0, UpdateMethod::ASYNCHRONOUS, UpdateType::PUBLISH);
    _varRestart = new BoolVariable("restart", false, UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);
    _varOta = new StringVariable("ota", "", UpdateMethod::ASYNCHRONOUS, UpdateType::SUBSCRIBE);

    ICloudVariable* defaults[] = {_varLog, _varVersion, _varStatus, _varRestart, _varOta};
    for (auto* v : defaults) {
        v->setTypePrefix('d'); // "def" topic type
        _defaultVariables.push_back(v);
        _variables.push_back(v);
    }

    // restart -> reboot the module
    _varRestart->onReceive([](const bool& value) {
        if (value) {
            ESP_LOGW(TAG, "Restart command received, rebooting");
            esp_restart();
        }
    });

    // ota -> parse {version,url} and log (no download in this iteration)
    _varOta->onReceive([](const std::string& payload) {
        cJSON* root = cJSON_Parse(payload.c_str());
        if (root) {
            cJSON* version = cJSON_GetObjectItem(root, "version");
            cJSON* url = cJSON_GetObjectItem(root, "url");
            ESP_LOGI(TAG, "OTA request received - version: %s, url: %s",
                     cJSON_IsString(version) ? version->valuestring : "?",
                     cJSON_IsString(url) ? url->valuestring : "?");
            cJSON_Delete(root);
        } else {
            ESP_LOGW(TAG, "OTA payload is not valid JSON: %s", payload.c_str());
        }
    });

    // Publish the firmware version
    char version[32] = {0};
    Board::getSoftwareVersion(version);
    _varVersion->setValue(std::string(version));
}

void Cloud::log(const std::string& message) {
    if (_varLog) {
        _varLog->setValue(message);
    }
}

void Cloud::setStatus(int status) {
    if (_varStatus) {
        _varStatus->setValue(status);
    }
}

/* ------------------------------------------------------------------------- */
/* NVS credential persistence                                                */
/* ------------------------------------------------------------------------- */

bool Cloud::_loadCredentials(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return false;
    }

    auto readStr = [&](const char* key, std::string& out) -> bool {
        size_t len = 0;
        if (nvs_get_str(handle, key, nullptr, &len) != ESP_OK || len == 0) {
            return false;
        }
        std::vector<char> buf(len);
        if (nvs_get_str(handle, key, buf.data(), &len) != ESP_OK) {
            return false;
        }
        out = buf.data();
        return true;
    };

    std::string uuid, token, plat;
    bool ok = readStr(NVS_KEY_UUID, uuid) &&
              readStr(NVS_KEY_TOKEN, token) &&
              readStr(NVS_KEY_PLATFORM, plat);
    nvs_close(handle);

    if (!ok) {
        return false;
    }
    if (plat != _platformUuid) {
        ESP_LOGW(TAG, "Stored platform UUID mismatch, re-provisioning required");
        return false;
    }

    _deviceUuid = uuid;
    _deviceToken = token;
    ESP_LOGI(TAG, "Loaded device credentials from NVS (uuid: %s)", _deviceUuid.c_str());
    return true;
}

void Cloud::_saveCredentials(void) {
    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS to save credentials");
        return;
    }
    nvs_set_str(handle, NVS_KEY_UUID, _deviceUuid.c_str());
    nvs_set_str(handle, NVS_KEY_TOKEN, _deviceToken.c_str());
    nvs_set_str(handle, NVS_KEY_PLATFORM, _platformUuid.c_str());
    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "Saved device credentials to NVS");
}

/* ------------------------------------------------------------------------- */
/* Background task / state machine                                           */
/* ------------------------------------------------------------------------- */

void Cloud::_task(void* arg) {
    Cloud* self = static_cast<Cloud*>(arg);
    self->_run();
    // _run() only returns on a terminal error state; clear the handle so end()
    // does not delete an already-deleted task.
    self->_taskHandle = nullptr;
    vTaskDelete(nullptr);
}

void Cloud::_run(void) {
    // 1. Determine credentials
    if (_haveCredentials) {
        _state = CloudState::CONNECTING;
    } else {
        _state = CloudState::LOADING_CREDS;
        if (_loadCredentials()) {
            _state = CloudState::CONNECTING;
        } else {
            _state = CloudState::PROVISION_CREATE;
        }
    }

    // 2. Provisioning (POST /device)
    while (_state == CloudState::PROVISION_CREATE) {
        DeviceCredentials creds;
        int status = CloudProvisioning::createDevice(_host, _platformUuid, _platformToken,
                                                     _projectId, _deviceName, creds);
        if (status == 201) {
            _deviceUuid = creds.uuid;
            _deviceToken = creds.token;
            _state = CloudState::PROVISION_PENDING;
        } else if (status == 409) {
            ESP_LOGE(TAG, "Device already created but credentials are lost. "
                          "Delete the device on the platform to re-provision.");
            _state = CloudState::ERR_ALREADY_CREATED;
            return;
        } else {
            ESP_LOGW(TAG, "Device creation failed (status %d), retrying", status);
            vTaskDelay(pdMS_TO_TICKS(PROVISION_RETRY_MS));
        }
    }

    // 3. Wait for user acceptance (GET /status)
    while (_state == CloudState::PROVISION_PENDING) {
        DeviceStatus st = {false, false};
        int status = CloudProvisioning::getStatus(_host, _platformUuid, _deviceUuid,
                                                  _deviceToken, st);
        if (status == 200) {
            if (!st.pending && st.accepted) {
                _saveCredentials();
                _state = CloudState::CONNECTING;
            } else if (!st.pending && !st.accepted) {
                ESP_LOGE(TAG, "Device was rejected by the user");
                _state = CloudState::ERR_REJECTED;
                return;
            } else {
                ESP_LOGI(TAG, "Device pending acceptance...");
                vTaskDelay(pdMS_TO_TICKS(STATUS_POLL_MS));
            }
        } else {
            ESP_LOGW(TAG, "Status request failed (status %d), retrying", status);
            vTaskDelay(pdMS_TO_TICKS(STATUS_POLL_MS));
        }
    }

    // 4. Connect to MQTT broker
    std::string uri = "wss://" + _host + "/mqtt";
    MQTTManager* mqtt = MQTTManager::getInstance();
    if (!mqtt->init(uri.c_str(), _deviceUuid.c_str(), _deviceToken.c_str()) || !mqtt->connect()) {
        ESP_LOGE(TAG, "Failed to start MQTT client");
        _state = CloudState::IDLE;
        return;
    }

    // Register subscriptions (deferred until MQTT_EVENT_CONNECTED re-subscribes them)
    for (auto* var : _variables) {
        _subscribeVariable(var);
    }

    _state = CloudState::CONNECTED;
    ESP_LOGI(TAG, "Cloud connected");

    // 5. Service variables: publish according to their refresh policy
    while (true) {
        uint32_t now = cloudMillis();
        if (mqtt->isConnected()) {
            for (auto* var : _variables) {
                if (var->shouldPublish(now)) {
                    mqtt->publish(_topicFor(var), var->serialize());
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(SERVICE_PERIOD_MS));
    }
}
