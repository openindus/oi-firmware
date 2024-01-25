#include <string.h>
#include "esp_log.h"
#include "Cloud.hpp"
#include "Cloud_Private.hpp"

/// @brief Subscribes to all variables mqtt topic
void Cloud::_updateSubscriptions(void)
{
    char ota_topic[64] = {'\0'};
    VarMaster *var = this->_var_list;

    strcat((char *) &ota_topic, this->_UUID);
    strcat((char *) &ota_topic, Cloud_OTA_topic);
    strcat((char *) &ota_topic, Mqtt_Topic_Receive_Suffix);
    if (esp_mqtt_client_subscribe(this->_mqtt_client, ota_topic, 2) == -1)
        ESP_LOGE(Cloud_LogTag, "Cloud::_updateSubscriptions(): Failed to subscribe to firmware version topic");
    if (var != NULL) {
        while (var != NULL) {
            if (esp_mqtt_client_subscribe(this->_mqtt_client, var->_receive_topic, 2) == -1)
                ESP_LOGE(Cloud_LogTag, "Cloud::_updateSubscriptions(): Failed to subscribe to topic %s", var->_name);
            if (var->_next == NULL)
                break;
            var = var->_next;
        }
    }
}

/// @brief Publish a given variable
/// @param variable
void Cloud::_publishVar(VarMaster *variable)
{
    unsigned char *data = NULL;
    unsigned short data_length = 0;

    if (this->_is_init == false) {
        ESP_LOGE(Cloud_LogTag, "Cloud::_publishVar(): Publish variable failed, not initialized");
        return;
    };
    data = variable->getDataAsBytes();
    data_length = variable->getDataByteSize();
    if (data == NULL) {
        return;
    }
    if (xSemaphoreTakeFromISR(this->_lock_mutex, NULL) != pdTRUE) {
        if (data != NULL)
            free(data);
        return;
    }
    if (this->_mqtt_client == NULL) {
        xSemaphoreGiveFromISR(this->_lock_mutex, NULL);
        if (data != NULL)
            free(data);
        return;
    }
    variable->_state_changed = false;
    ESP_LOGI(Cloud_LogTag, "Cloud::_publishVar(): Sending payload on topic %s", variable->_name);
    xSemaphoreGiveFromISR(this->_lock_mutex, NULL);
    if (esp_mqtt_client_publish(this->_mqtt_client, variable->_publish_topic, (char *) data, data_length, Mqtt_QOS, 0) == -1)
        ESP_LOGW(Cloud_LogTag, "Cloud::_publishVar(): Publish variable %s failed", variable->_name);
    if (data != NULL)
        free(data);
}

/// @brief Updates a variable received by the mqtt client.
/// If a message is received on the OTA topic, it starts the
/// update by calling _ota_update()
void Cloud::_setVarFromServer(char *topic, char *data, int data_len, int topic_len)
{
    if (data_len >= Mqtt_Max_Buffer_Size)
        return;
    VarMaster *var = this->_var_list;
    char topic_chr[topic_len + 1] = {'\0'};
    char data_str[data_len + 1] = {'\0'};

    strncpy(topic_chr, topic, topic_len);
    strncpy(data_str, data, data_len);
    if (strstr(topic_chr, "OTA") != NULL) {
        this->_ota_update();
        return;
    }
    if (this->_var_list == NULL)
        return;
    while (var->_next != NULL && strcmp(var->_receive_topic, topic_chr) != 0)
        var = var->_next;
    if (var->_is_writable == false)
        return;
    var->updateData(data_str);
    var->triggerCallback();
    this->_publishVar(var);
}
