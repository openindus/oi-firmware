/**
 * @file CloudProvisioning.cpp
 * @brief Device provisioning against the OI-Cloud REST API (create + status).
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CloudProvisioning.hpp"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "cJSON.h"

static const char* TAG = "CloudProvisioning";

namespace {

/**
 * @brief HTTP event handler accumulating the response body into a std::string.
 */
esp_err_t httpEventHandler(esp_http_client_event_t* evt) {
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->user_data) {
        auto* body = static_cast<std::string*>(evt->user_data);
        body->append(static_cast<const char*>(evt->data), evt->data_len);
    }
    return ESP_OK;
}

/**
 * @brief Perform an HTTP request and collect status + body.
 * @return HTTP status code, or -1 on transport error.
 */
int performRequest(const std::string& url, esp_http_client_method_t method,
                   const std::string* body, const std::string* bearer,
                   std::string& responseBody) {
    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.method = method;
    config.event_handler = httpEventHandler;
    config.user_data = &responseBody;
    config.crt_bundle_attach = esp_crt_bundle_attach;
    config.timeout_ms = 10000;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to init HTTP client");
        return -1;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    std::string authHeader;
    if (bearer) {
        authHeader = "Bearer " + *bearer;
        esp_http_client_set_header(client, "Authorization", authHeader.c_str());
    }
    if (body) {
        esp_http_client_set_post_field(client, body->c_str(), body->length());
    }

    int status = -1;
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "%s %s -> %d", (method == HTTP_METHOD_POST ? "POST" : "GET"),
                 url.c_str(), status);
    } else {
        ESP_LOGE(TAG, "HTTP request to %s failed: %s", url.c_str(), esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return status;
}

} // namespace

namespace CloudProvisioning {

int createDevice(const std::string& host, const std::string& platformUuid,
                 const std::string& platformToken, int projectId,
                 const std::string& deviceName, DeviceCredentials& out) {
    std::string url = "https://" + host + "/api/v1/plateform/" + platformUuid + "/device";

    // Build request body
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", deviceName.c_str());
    cJSON_AddNumberToObject(root, "project_id", projectId);
    cJSON_AddStringToObject(root, "platform_token", platformToken.c_str());
    char* bodyStr = cJSON_PrintUnformatted(root);
    std::string body = bodyStr ? bodyStr : "";
    cJSON_free(bodyStr);
    cJSON_Delete(root);

    std::string response;
    int status = performRequest(url, HTTP_METHOD_POST, &body, nullptr, response);

    if (status == 201) {
        cJSON* resp = cJSON_Parse(response.c_str());
        if (resp) {
            cJSON* uuid = cJSON_GetObjectItem(resp, "uuid");
            cJSON* token = cJSON_GetObjectItem(resp, "token");
            if (cJSON_IsString(uuid) && cJSON_IsString(token)) {
                out.uuid = uuid->valuestring;
                out.token = token->valuestring;
            } else {
                ESP_LOGE(TAG, "Create response missing uuid/token");
                status = -1;
            }
            cJSON_Delete(resp);
        } else {
            ESP_LOGE(TAG, "Failed to parse create response");
            status = -1;
        }
    }

    return status;
}

int getStatus(const std::string& host, const std::string& platformUuid,
              const std::string& deviceUuid, const std::string& token,
              DeviceStatus& out) {
    std::string url = "https://" + host + "/api/v1/plateform/" + platformUuid +
                      "/device/" + deviceUuid + "/status";

    std::string response;
    int status = performRequest(url, HTTP_METHOD_GET, nullptr, &token, response);

    if (status == 200) {
        cJSON* resp = cJSON_Parse(response.c_str());
        if (resp) {
            cJSON* pending = cJSON_GetObjectItem(resp, "pending");
            cJSON* accepted = cJSON_GetObjectItem(resp, "accepted");
            out.pending = cJSON_IsTrue(pending);
            out.accepted = cJSON_IsTrue(accepted);
            cJSON_Delete(resp);
        } else {
            ESP_LOGE(TAG, "Failed to parse status response");
            status = -1;
        }
    }

    return status;
}

} // namespace CloudProvisioning
