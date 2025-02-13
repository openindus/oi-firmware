#include <sys/param.h>
#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_app_format.h"
#include "esp_crt_bundle.h"
#include "freertos/semphr.h"
#include "Cloud.hpp"
#include "Cloud_Private.hpp"

/// @brief Callback handling MQTT events from Cloud's mqtt client
void Cloud::mqtt_event_Handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    Cloud *cloud = (Cloud *) handler_args;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:
#if defined(OI_CORE)
            if (cloud->_monitor_status == true)
                CLOUD_CONNECTED();
#endif
            ESP_LOGI(Cloud_LogTag, "Cloud::update(): Connected to broker");
            cloud->_is_broker_connected = true;
            cloud->_updateSubscriptions();
            cloud->_forceUpdate();
            cloud->_sendDeviceInfo();
            break;
        case MQTT_EVENT_DISCONNECTED:
#if defined(OI_CORE)
            if (cloud->_monitor_status == true)
                CLOUD_DISCONNECTED();
#endif
            cloud->_is_broker_connected = false;
            ESP_LOGW(Cloud_LogTag, "Cloud::update(): Disconnected from broker");
            break;
        case MQTT_EVENT_DATA: {
            cloud->_setVarFromServer(event->topic, event->data, event->data_len, event->topic_len);
            break;
        }
        default :
            break;
    }
}

/// @brief Cloud object constructor
/// @param uuid Valid Device ID required
/// @param firmware_name Custom firmware name / version (16 bytes max, can be NULL)
Cloud::Cloud(const char *uuid, const char *firmware_name)
{
    char *lwt_topic = NULL;
    char *nvs_uuid = getNVS(_UUID_Key);
    unsigned short uuid_size = 0;

    this->_lock_mutex = xSemaphoreCreateBinary();
    this->_UUID = NULL;
    if (uuid == NULL && nvs_uuid == NULL) {
        ESP_LOGE(Cloud_LogTag, "Cloud(): Fatal: Undefined Machine UUID");
        abort();
    }
    if (uuid == NULL) {
        uuid_size = strlen(nvs_uuid) + 1;
        this->_UUID = new char[uuid_size];
        memset(this->_UUID, 0, uuid_size);
        strcpy(this->_UUID, nvs_uuid);
    } else {
        uuid_size = strlen(uuid) + 1;
        this->_UUID = new char[uuid_size];
        memset(this->_UUID, 0, uuid_size);
        strcpy(this->_UUID, uuid);
    }
    if (nvs_uuid == NULL || strcmp(nvs_uuid, this->_UUID) != 0) {
        saveNVS(_Refresh_Token_Key, NULL);
        saveNVS(_UUID_Key, this->_UUID);
    }
    delete nvs_uuid;
    saveNVS(_Access_Token_Key, NULL);
    lwt_topic = new char[strlen(this->_UUID) + strlen(Mqtt_Last_Will_Topic_Suffix) + 1];
    strcpy(lwt_topic, this->_UUID);
    strcat(lwt_topic, Mqtt_Last_Will_Topic_Suffix);
    memset(&this->_topic_firmware, 0, sizeof(_topic_firmware));
    memset(&this->_firmware_name, 0, sizeof(_topic_firmware));
    strcat((char *) &this->_topic_firmware, this->_UUID);
    strcat((char *) &this->_topic_firmware, Cloud_OTA_topic);
    strcat((char *) &this->_topic_firmware, Mqtt_Topic_Send_Suffix);
    memset(&this->_mqtt_cfg, 0, sizeof(esp_mqtt_client_config_t));
    if (firmware_name != NULL)
        strncpy((char *) &this->_firmware_name, firmware_name, 16);
    this->_mqtt_cfg.uri = Mqtt_URI;
    this->_mqtt_cfg.use_global_ca_store = false;
    this->_mqtt_cfg.network_timeout_ms = Mqtt_Timeout * 1000;
    this->_mqtt_cfg.reconnect_timeout_ms = Mqtt_Timeout * 1000;
    this->_mqtt_cfg.disable_auto_reconnect = false;
    this->_mqtt_cfg.disable_clean_session = false;
    this->_mqtt_cfg.port = Mqtt_Port;
    this->_mqtt_cfg.network_timeout_ms = Mqtt_Timeout * 1000;
    this->_mqtt_cfg.disable_auto_reconnect = false;
    this->_mqtt_cfg.protocol_ver = MQTT_PROTOCOL_V_3_1_1;
    this->_mqtt_cfg.client_id = NULL;
    this->_mqtt_cfg.username = NULL;
    this->_mqtt_cfg.password = NULL;
    this->_mqtt_cfg.lwt_msg = Mqtt_Last_Will_Msg;
    this->_mqtt_cfg.lwt_qos = 2;
    this->_mqtt_cfg.lwt_topic = lwt_topic;
    this->_mqtt_cfg.keepalive = Mqtt_Timeout * 2;
    this->_is_authenticated = false;
    this->_auth_task = NULL;
    this->_mqtt_cfg.skip_cert_common_name_check = Mqtt_Skip_Name_Check;
    this->_mqtt_cfg.crt_bundle_attach = (Mqtt_Use_TLS == true) ? esp_crt_bundle_attach : NULL;
    this->_var_list = NULL;
    this->_is_broker_connected = false;
    this->_is_init = false;
    this->_is_paused = false;
    this->_last_update_time = 0;
    this->_monitor_status = false;
    xSemaphoreGiveFromISR(this->_lock_mutex, NULL);
}

/// @brief Clears mqtt authentication related configuration, it also
/// destroys the mqtt client itself.
void Cloud::_cleanConfig(void)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::_cleanConfig");
    if (this->_mqtt_client != NULL) {
        esp_mqtt_client_stop(this->_mqtt_client);
        esp_mqtt_client_destroy(this->_mqtt_client);
        this->_mqtt_client = NULL;
#if defined(OI_CORE)
        if (this->_monitor_status == true)
            CLOUD_DISCONNECTED();
#endif
    }
    if (this->_mqtt_cfg.password != NULL) {
        delete this->_mqtt_cfg.password;
        this->_mqtt_cfg.password = NULL;
    }
}

/// @brief Cloud object destructor
Cloud::~Cloud(void)
{
    VarMaster *var = this->_var_list;
    VarMaster *temp = this->_var_list;

    if (this->_auth_task != NULL) {
        vTaskDelete(*this->_auth_task);
        delete this->_auth_task;
        this->_auth_task = NULL;
    }
    if (this->_UUID != NULL) {
        delete this->_UUID;
        this->_UUID = NULL;
    }
    if (this->_var_list != NULL) {
        while (var != NULL) {
            temp = var;
            var = var->_next;
            delete temp;
        }
    }
    if (this->_mqtt_client != NULL) {
        esp_mqtt_client_destroy(this->_mqtt_client);
        this->_mqtt_client = NULL;
    }
}

void Cloud::init(void)
{
    if (this->_lock_mutex == NULL) {
        ESP_LOGE(Cloud_LogTag, "Failed to create global mutex");
        abort();
    }
    if (this->_is_init == true) {
        ESP_LOGW(Cloud_LogTag, "Cloud::init(): Cloud was already initialized");
        return;
    }
    this->_is_authenticated = false;
    this->_is_broker_connected = false;
    this->_is_init = false;
    this->_is_paused = false;
    if (this->_auth_task == NULL) {
        this->_auth_task = new TaskHandle_t;
        xTaskCreate(task_authenticate, "Cloud_auth_agent", 8192, this, 10, this->_auth_task);
    }
    if (this->_ntp_task == NULL) {
        this->_ntp_task = new TaskHandle_t;
        xTaskCreate(updateSytemTimeNTP, "Cloud_ntp_client", 8192, this, 11, this->_ntp_task);
    }
}

void Cloud::_forceUpdate(void)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::_forceUpdate");
    VarMaster *var_lst = this->_var_list;

    while (var_lst != NULL) {
        var_lst->_state_changed = false;
        var_lst->_timer = 0.0f;
        var_lst->resetDeltaTemp();
        this->_publishVar(var_lst);
        var_lst = var_lst->_next;
    }
}

bool Cloud::update(void)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::update");
    VarMaster *var_lst = this->_var_list;
    float elapsed_time = 0.0f;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    if (var_lst == NULL || this->_is_paused == true)
        return (true);
    if (this->_mqtt_client == NULL) {
        return (false);
    }
    if (this->_is_init == false) {
        ESP_LOGW(Cloud_LogTag, "Cloud::update(): Update failed, not initialized (yet)");
        return (false);
    }
    if (this->_is_broker_connected == false) {
        ESP_LOGW(Cloud_LogTag, "Cloud::update(): The client is not ready (not connected to broker)");
        return (false);
    }
    if (tm.tm_year + 1900 < 2023) {
        ESP_LOGW(Cloud_LogTag, "Cloud::update: System time is not synced yet.");
        return (false);
    }
    elapsed_time = (float) (clock() - this->_last_update_time) / CLOCKS_PER_SEC;
    this->_last_update_time = clock();
    while (var_lst != NULL) {
        if (var_lst->canUpdateValue(elapsed_time) == true) {
            var_lst->_state_changed = false;
            var_lst->_timer = 0.0f;
            var_lst->resetDeltaTemp();
            this->_publishVar(var_lst);
        }
        var_lst = var_lst->_next;
    }
    return (true);
}

/// @brief Send device firmware infos
void Cloud::_sendDeviceInfo(void)
{
    char message_version[96] {'\0'};
    char *ota_success_msg = getNVS(Cloud_OTA_status_key);

    if (this->_firmware_name[0] != '\0') {
        strncat((char *) &message_version, this->_firmware_name, 16);
        strcat((char *) &message_version, " /  ");
    }
    strncat((char *) &message_version, esp_ota_get_app_description()->version, 16);
    strcat((char *) &message_version, " / IDF ");
    strncat((char *) &message_version, esp_ota_get_app_description()->idf_ver, 16);
    if (ota_success_msg != NULL) {
        strncat(message_version, ota_success_msg , 32);
        delete ota_success_msg;
    }
    if (esp_mqtt_client_publish(this->_mqtt_client, this->_topic_firmware, message_version, 0, 1, 0) == -1)
        ESP_LOGW(Cloud_LogTag, "Cloud::_sendDeviceInfo(): Publish firmware version failed");
}

/// @brief Do an OTA update of the firmware
/// The firmware sends a request to Cloud_OTA_route
/// to get the new firmware and applies the update
void Cloud::_ota_update(void)
{
    char *token = getToken(this->_UUID);
    esp_http_client_config_t http_conf;
    esp_err_t err = 0;

    if (token == NULL) {
        ESP_LOGE(Cloud_LogTag, "OTA: No token provided");
        return;
    }
    memset(&http_conf, 0, sizeof(esp_http_client_config_t));
    http_conf.method = HTTP_METHOD_POST;
    http_conf.skip_cert_common_name_check = HTTP_Skip_Name_Check;
    http_conf.transport_type = (HTTP_Use_TLS == true) ? HTTP_TRANSPORT_OVER_SSL : HTTP_TRANSPORT_OVER_TCP;
    http_conf.event_handler = NULL;
    http_conf.port = HTTP_Port;
    http_conf.host = HTTP_Host;
    http_conf.path = Cloud_OTA_route;
    http_conf.auth_type = HTTP_AUTH_TYPE_BASIC;
    http_conf.crt_bundle_attach = (HTTP_Use_TLS == true) ? esp_crt_bundle_attach : NULL;
    http_conf.username = "CloudOTA";
    http_conf.password = token;
#if defined(OI_CORE)
    CLOUD_OTA();
#endif
    err = esp_https_ota(&http_conf);
    delete token;
    if (err != ESP_OK) {
#if defined(OI_CORE)
        CLOUD_CONNECTED();
#endif
        ESP_LOGE(Cloud_LogTag, "OTA: Update failed with code %d: %s", err, esp_err_to_name(err));
        saveNVS(Cloud_OTA_status_key, ". OTA update failed");
        this->_sendDeviceInfo();
    } else {
        ESP_LOGI(Cloud_LogTag, "OTA: Update sucessfull, restarting...");
        saveNVS(Cloud_OTA_status_key, ". OTA update succeeded");
        this->_sendDeviceInfo();
        esp_restart();
    }
}

bool Cloud::addVar(VarMaster *variable)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::addVar");
    VarMaster *lst = this->_var_list;
    char uuid_topic[strlen(this->_UUID) + 2] = {'\0'};

    strcpy((char *) &uuid_topic, this->_UUID);
    strcat((char *) &uuid_topic, "/");
    if (lst != NULL) {
        while (lst != NULL) {
            if (strcmp(lst->_name, variable->_name) == 0 || lst == variable) {
                ESP_LOGW(Cloud_LogTag, "Cloud::addVar(): Variable %s already added, ignoring", variable->_name);
                return (false);
            }
            if (lst->_next == NULL)
                break;
            if (lst->_next == this->_var_list)
                return (false);
            lst = lst->_next;
        }
        variable->_next = NULL;
        variable->_prev = lst;
        lst->_next = variable;
    } else {
        this->_var_list = variable;
        lst = this->_var_list;
    }
    variable->setTopic((char *) &uuid_topic);
    if (this->_is_init == false)
        return (true);
    if (esp_mqtt_client_subscribe(this->_mqtt_client, lst->_receive_topic, Mqtt_QOS) == -1) {
        ESP_LOGW(Cloud_LogTag, "Cloud::addVar(): Failed to subscribe to topic %s", lst->_name);
        return (false);
    }
    return (true);
}

bool Cloud::removeVar(char const *variable_name, bool destroy)
{
    VarMaster *lst = this->_var_list;

    if (lst == NULL) {
        ESP_LOGW(Cloud_LogTag, "Cloud::removeVar(): Trying to remove variable an unknown variable");
        return (false);
    }
    while (lst != NULL && strcmp(lst->_name, variable_name) != 0)
        lst = lst->_next;
    if (lst == NULL) {
        ESP_LOGW(Cloud_LogTag, "Cloud::removeVar(): Trying to remove variable %s, but it was not found", variable_name);
        return (false);
    }
    if (lst == this->_var_list && lst->_next == NULL) {
        this->_var_list = NULL;
    } else {
        if (lst->_prev != NULL) {
            lst->_prev->_next = lst->_next;
            if (lst->_next != NULL)
                lst->_next->_prev = lst->_prev;
        } else {
            if (lst->_next != NULL)
                this->_var_list = lst->_next;
        }
    }
    lst->_next = NULL;
    lst->_prev = NULL;
    if (destroy == true)
        delete lst;
    return (true);
}

bool Cloud::removeVar(VarAny *variable, bool destroy)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::removeVar");
    if (this->_var_list == NULL) {
        ESP_LOGW(Cloud_LogTag, "Cloud::removeVar(): Trying to remove variable in an empty stack");
        return (false);
    }
    if (variable == NULL) {
        ESP_LOGE(Cloud_LogTag, "Cloud::removeVar(): Invalid NULL pointer");
        return (false);
    }
    if (variable == this->_var_list && variable->_next == NULL) {
        this->_var_list = NULL;
    } else {
        if (variable->_prev != NULL) {
            variable->_prev->_next = variable->_next;
            if (variable->_next != NULL)
                variable->_next->_prev = variable->_prev;
        } else {
            if (variable->_next != NULL)
                this->_var_list = variable->_next;
        }
    }
    variable->_next = NULL;
    variable->_prev = NULL;
    if (destroy == true)
        delete variable;
    return (true);
}

/// @brief Wait for the Cloud's lock to be available
/// @param timeout_ms Timeout
/// @return True on success, False on timeout
bool Cloud::_cloudLockAcquire(unsigned int timeout_ms)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::_cloudLockAcquire");
    BaseType_t res = 0;

    do {
        res = xSemaphoreTakeFromISR(this->_lock_mutex, NULL);
        vTaskDelay(pdMS_TO_TICKS(1));
        timeout_ms--;
        if (timeout_ms == 0)
            return (false);
    } while (res != pdTRUE);
    return (true);
}

void Cloud::pause(void)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::pause");
    if (this->_cloudLockAcquire(1000) == false) {
        ESP_LOGE(Cloud_LogTag, "Cloud::pause(): Failed to pause: unable to acquire the lock, timeout");
        return;
    }
    if (this->_is_paused == false) {
        this->_cleanConfig();
        this->_is_init = false;
        this->_is_broker_connected = false;
        this->_is_paused = true;
    }
    xSemaphoreGiveFromISR(this->_lock_mutex, NULL);
}

void Cloud::resume(void)
{
    ESP_LOGD(Cloud_LogTag, "Cloud::resume");
    if (this->_is_paused == true) {
        if (this->_cloudLockAcquire(1000) == false) {
            ESP_LOGE(Cloud_LogTag, "Cloud::resume(): Failed to resume: unable to acquire the lock, timeout");
            return;
        }
        this->_is_paused = false;
        xSemaphoreGiveFromISR(this->_lock_mutex, NULL);
        this->init();
    }
}

bool Cloud::isInit(void)
{
    return (this->_is_init);
}

bool Cloud::isConnected(void)
{
    return (this->_is_broker_connected);
}

bool Cloud::isPaused(void)
{
    return (this->_is_paused);
}

#if defined(OI_CORE)
void Cloud::monitorStatus(void)
{
    this->_monitor_status = true;
}
#endif