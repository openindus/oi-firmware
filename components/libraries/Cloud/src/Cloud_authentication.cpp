#include "Cloud_Private.hpp"
#include "Cloud.hpp"
#include "esp_log.h"
#include "nvs_flash.h"

/// @brief This function checks if the access token is expired by making
/// an HTTP request to the HTTP_Token_Check_Route route.
/// @param access_token Access token
/// @return True or False
static bool hasAccessTokenExpired(const char *access_token)
{
    unsigned short response_code = 0;
    char *response_str = NULL;

    if (access_token == NULL)
        return (true);
    response_str = http_get_bearer(HTTP_Token_Check_Route, access_token, &response_code);
    if (response_str != NULL)
        delete response_str;
    if (response_code != 200)
        return (true);
    return (false);
}

/// @brief Retreives a value from the NVS
/// @param key Key
/// @return NULL if the key was not found or a new-allocated string
/// containing the value found. You must call delete after user.
char *getNVS(const char *key)
{
    char *token = new char[_Token_BUFFER_SIZE] {'\0'};
    size_t size = _Token_BUFFER_SIZE;
    esp_err_t error = nvs_flash_init();
    nvs_handle_t nvs_handle;

    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    error = nvs_open("nvs", NVS_READONLY, &nvs_handle);
    error = nvs_get_str(nvs_handle, key, token, &size);
    if (error != ESP_OK) {
        if (error == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(Cloud_LogTag, "Key not found in NVS");
        } else {
            ESP_LOGW(Cloud_LogTag, "An error occured while reading value in memory : %s", esp_err_to_name(error));
        }
        nvs_close(nvs_handle);
        delete token;
        return (NULL);
    }
    nvs_close(nvs_handle);
    return (token);
}

/// @brief Saves a value in the NVS. If value is NULL then it performs a
/// earase operation.
/// @param key Key
/// @param value Value
/// @return 0 on success / 1 on error
int saveNVS(const char *key, const char *value)
{
    esp_err_t error = nvs_flash_init();
    nvs_handle_t nvs_handle;

    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    error = nvs_open("nvs", NVS_READWRITE, &nvs_handle);
    if (error != ESP_OK) {
        ESP_LOGE(Cloud_LogTag, "Failed to open nvs flash memory : %s", esp_err_to_name(error));
        return (1);
    }
    if (value == NULL) {
        error = nvs_erase_key(nvs_handle, key);
    } else {
        error = nvs_set_str(nvs_handle, key, value);
    }
    if (error != ESP_OK) {
        ESP_LOGE(Cloud_LogTag, "Failed to save value in memory : %s", esp_err_to_name(error));
        return (1);
    }
    error = nvs_commit(nvs_handle);
    if (error != ESP_OK) {
        ESP_LOGE(Cloud_LogTag, "Failed to save chages : %s", esp_err_to_name(error));
        return (1);
    }
    nvs_close(nvs_handle);
    return (0);
}

/// @brief This function authenticate the device by retreiving the access token.
/// If the refresh token is not saved in the NVS, it will also performs the
/// initial setup (sending of the UUID) of the device and saves the refresh
/// token in the NVS.
/// @param uuid Device ID
/// @return NULL on error, or a new-allocated string containing the access token.
/// You must call delete after user.
char *getToken(const char *uuid)
{
    unsigned short status = 0;
    char *token = getNVS(_Access_Token_Key);
    char *refresh_token = getNVS(_Refresh_Token_Key);

    if (refresh_token == NULL || hasAccessTokenExpired(token) == true) {
        if (token != NULL)
            delete token;
        if (refresh_token == NULL) {
            refresh_token = http_get_bearer(HTTP_Broker_Init_Route, uuid, &status);
            if (refresh_token == NULL) {
                ESP_LOGW(Cloud_LogTag, "Authentication: Invalid credentials, check your internet connection and your credentials");
                return (NULL);
            }
            saveNVS(_Refresh_Token_Key, refresh_token);
        }
        token = http_get_bearer(HTTP_Token_Refresh_Route, refresh_token, &status);
        delete refresh_token;
        if (status != 200) {
            if (token != NULL)
                delete token;
            ESP_LOGE(Cloud_LogTag, "Authentication: Invalid credentials, check your internet connection and your credentials");
            return (NULL);
        }
        saveNVS(_Access_Token_Key, token);
    } else if (refresh_token != NULL) {
        delete refresh_token;
    }
    return (token);
}

/// @brief Performs all steps of the authentication process and restarts the
/// mqtt client with the new configuration.
/// This function tries to authenticate the device.
/// If the process fails: it sets the cloud's authenticated
/// and init status to false.
/// Else: it updates the mqtt client's configuration and performs a reset
/// with the new credentials, it also sets the cloud's authenticated and
/// init status to true.
/// @param cloud Cloud instance
void cloud_authenticate(Cloud *cloud)
{
    char *auth_token = NULL;

#if defined(CONFIG_CORE)
    if (cloud->_monitor_status == true)
        CLOUD_AUTHENTICATING();
#endif
    auth_token = getToken(cloud->_UUID);
    if (auth_token == NULL) {
        ESP_LOGW(Cloud_LogTag, "Authentication: Authentication failed");
        cloud->_is_authenticated = false;
        cloud->_is_init = false;
#if defined(CONFIG_CORE)
        if (cloud->_monitor_status == true)
            CLOUD_DISCONNECTED();
#endif
        return;
    } else {
        if (cloud->_is_broker_connected == true)
           cloud->_cleanConfig();
        cloud->_mqtt_cfg.password = auth_token;
        cloud->_mqtt_cfg.username = cloud->_UUID;
        cloud->_mqtt_cfg.client_id = cloud->_UUID;
        cloud->_mqtt_client = esp_mqtt_client_init(&cloud->_mqtt_cfg);
        if (cloud->_mqtt_client == NULL) {
            ESP_LOGE(Cloud_LogTag, "Failed to create MQTT client.");
            return;
        }
        esp_mqtt_client_register_event(cloud->_mqtt_client, MQTT_EVENT_ANY, Cloud::mqtt_event_Handler, cloud);
        esp_mqtt_client_start(cloud->_mqtt_client);
        cloud->_is_authenticated = true;
        cloud->_is_init = true;
    }
}

/// @brief Refreshes the access token.
/// It earases the current access token, and tries to perform the
/// authentication process.
/// @param cloud Cloud instance
void update_access_token(Cloud *cloud)
{
    ESP_LOGI(Cloud_LogTag, "Authentication: Refreshing authentication creadentials...");
    saveNVS(_Access_Token_Key, NULL);
    cloud->_is_authenticated = false;
    cloud->_is_init = false;
    cloud->_cleanConfig();
    delete getToken(cloud->_UUID);
    cloud_authenticate(cloud);
}

/// @brief Background running authentication task, it performs the
/// authentication with the server when needed.
/// This function must be launched in a task because it never ends.
/// @param data Cloud Instance as void pointer
void task_authenticate(void *data)
{
    Cloud *cloud = (Cloud *) data;
    unsigned int auth_reset = 0;
    unsigned int token_timeout = 0;

    while (1) {
        if (cloud->_is_broker_connected == true && token_timeout < (
                HTTP_Token_Validity / Cloud_Auth_retry_delay) * 10) {
                token_timeout++;
        } else {
            token_timeout = 0;
        }
        if (token_timeout >= (HTTP_Token_Validity / Cloud_Auth_retry_delay) * 10 &&
                cloud->_is_broker_connected == true) {
                update_access_token(cloud);
                vTaskDelay(pdMS_TO_TICKS(Cloud_Auth_retry_delay * 1000));
        }
        if (cloud->_is_authenticated == false && cloud->_is_init == false &&
                cloud->_is_paused == false) {
            ESP_LOGI(Cloud_LogTag, "Authentication: Not authenticated, retrying...");
            cloud_authenticate(cloud);
            vTaskDelay(pdMS_TO_TICKS(Cloud_Auth_retry_delay * 1000));
            continue;
        }
        if (cloud->_is_broker_connected == false &&
                auth_reset < (Cloud_Auth_timeout / Cloud_Auth_retry_delay) * 10) {
            auth_reset++;
        } else {
            auth_reset = 0;
        }
        if (auth_reset >= (Cloud_Auth_timeout / Cloud_Auth_retry_delay) * 10 &&
                cloud->_is_paused == false) {
            cloud->_is_authenticated = false;
            cloud->_is_init = false;
            if (cloud->_cloudLockAcquire(2000)) {
                cloud->_cleanConfig();
                ESP_LOGI(Cloud_LogTag, "Authentication: The session has expired");
            } else {
                ESP_LOGE(Cloud_LogTag, "Failed to acquire lock");
            }
            xSemaphoreGiveFromISR(cloud->_lock_mutex, NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(Cloud_Auth_retry_delay * 100));
    }
}
