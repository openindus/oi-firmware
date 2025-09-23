/**
 * @file Cloud.cpp
 * @brief Implementation file for the Cloud API component with MQTT support.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Cloud.hpp"
#include "MQTT/MQTTManager.hpp"
#include "CloudVariable.hpp"
#include "esp_mac.h"

static const char* TAG = "Cloud";

Cloud::Cloud(const char* platformToken, const char* macAddress) {
    ESP_LOGI(TAG, "Cloud instance created");
    
    if (!platformToken) {
        ESP_LOGE(TAG, "Platform token is required");
        return;
    }
    
    // Store platform token
    _platformToken = platformToken;
    ESP_LOGI(TAG, "Platform token set");
    
    // Store MAC address or get system MAC if not provided
    if (macAddress) {
        _macAddress = macAddress;
        ESP_LOGI(TAG, "Using provided MAC address: %s", _macAddress.c_str());
    } else {
        // Get system MAC address
        uint8_t mac[6];
        esp_err_t ret = esp_read_mac(mac, ESP_MAC_WIFI_STA);
        if (ret == ESP_OK) {
            char mac_str[18];
            snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            _macAddress = mac_str;
            ESP_LOGI(TAG, "Using system MAC address: %s", _macAddress.c_str());
        } else {
            ESP_LOGE(TAG, "Failed to get system MAC address");
            _macAddress = "00:00:00:00:00:00"; // Fallback
        }
    }
}

Cloud::~Cloud() {
    disconnect();
    ESP_LOGI(TAG, "Cloud instance destroyed");
}

bool Cloud::init(const char* brokerUrl) {
    ESP_LOGI(TAG, "Initializing Cloud API");
    
    if (!brokerUrl) {
        ESP_LOGE(TAG, "Broker URL is required for initialization");
        return false;
    }
    
    // Initialize MQTT manager with broker URL
    bool mqttResult = MQTTManager::getInstance()->init(brokerUrl);
    if (!mqttResult) {
        ESP_LOGE(TAG, "Failed to initialize MQTT manager");
        return false;
    }
    
    ESP_LOGI(TAG, "Cloud API initialized successfully");
    return true;
}

bool Cloud::connect(void) {
    ESP_LOGI(TAG, "Connecting to cloud service...");
    
    // Connect to MQTT broker
    bool connectResult = MQTTManager::getInstance()->connect();
    if (!connectResult) {
        ESP_LOGE(TAG, "Failed to connect to MQTT broker");
        return false;
    }
    
    ESP_LOGI(TAG, "Connected to cloud service");
    return true;
}

void Cloud::disconnect(void) {
    ESP_LOGI(TAG, "Disconnecting from cloud service...");
    MQTTManager::getInstance()->disconnect();
    ESP_LOGI(TAG, "Disconnected from cloud service");
}

void Cloud::update(void) {
    ESP_LOGV(TAG, "Cloud update cycle");
    // TODO
}

bool Cloud::isConnected(void) const {
    return MQTTManager::getInstance()->isConnected();
}

const std::string& Cloud::getPlatformToken(void) const {
    return _platformToken;
}

const std::string& Cloud::getMacAddress(void) const {
    return _macAddress;
}

template<>
void Cloud::registerVariable<bool>(CloudVariable<bool>& variable) {
    // TODO
}

template<>
void Cloud::registerVariable<int>(CloudVariable<int>& variable) {
    // TODO
}

template<>
void Cloud::registerVariable<float>(CloudVariable<float>& variable) {
    // TODO
}

template<>
void Cloud::registerVariable<std::string>(CloudVariable<std::string>& variable) {
    // TODO
}

void Cloud::unregisterVariable(const std::string& topicName) {
    // TODO
}