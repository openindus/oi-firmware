/**
 * @file MQTTManager.cpp
 * @brief Implementation file for MQTTManager class
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MQTTManager.hpp"

static const char* TAG = "MQTTManager";

// Static instance initialization
MQTTManager* MQTTManager::_instance = nullptr;

MQTTManager::MQTTManager() : _mqttClient(nullptr) {
    ESP_LOGI(TAG, "MQTTManager created");
}

MQTTManager::~MQTTManager() {
    disconnect();
    ESP_LOGI(TAG, "MQTTManager destroyed");
}

MQTTManager* MQTTManager::getInstance() {
    if (!_instance) {
        _instance = new MQTTManager();
    }
    return _instance;
}

bool MQTTManager::init(const char* brokerUrl) {
    if (_mqttClient) {
        ESP_LOGW(TAG, "MQTT client already initialized");
        return true;
    }
    
    if (!brokerUrl) {
        ESP_LOGE(TAG, "Broker URL is null");
        return false;
    }
    
    ESP_LOGI(TAG, "Initializing MQTT client with broker: %s", brokerUrl);
    
    // // Configure MQTT client
    // esp_mqtt_client_config_t mqtt_cfg = {};
    // mqtt_cfg.uri = brokerUrl;
    // mqtt_cfg.keepalive = 60;
    // mqtt_cfg.disable_auto_reconnect = false;
    // mqtt_cfg.disable_clean_session = false;
    
    // // Create MQTT client
    // _mqttClient = esp_mqtt_client_init(&mqtt_cfg);
    // if (!_mqttClient) {
    //     ESP_LOGE(TAG, "Failed to initialize MQTT client");
    //     return false;
    // }
    
    // // Register event handler
    // esp_mqtt_client_register_event(_mqttClient, MQTT_EVENT_ANY, _mqttEventHandler, this);
    
    ESP_LOGI(TAG, "MQTT client initialization completed");
    return true;
}

bool MQTTManager::connect(void) {
    // if (!_mqttClient) {
    //     ESP_LOGE(TAG, "MQTT client not initialized. Call init() first");
    //     return false;
    // }
    
    ESP_LOGI(TAG, "Connecting to MQTT broker");
    
    // esp_err_t result = esp_mqtt_client_start(_mqttClient);
    // if (result != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(result));
    //     return false;
    // }
    
    ESP_LOGI(TAG, "MQTT client connection initiated");
    return true;
}

void MQTTManager::disconnect(void) {
    // if (_mqttClient) {
    //     ESP_LOGI(TAG, "Disconnecting MQTT client");
    //     esp_mqtt_client_stop(_mqttClient);
    //     esp_mqtt_client_destroy(_mqttClient);
    //     _mqttClient = nullptr;
    //     _subscriptions.clear();
    // }
}

bool MQTTManager::isConnected(void) const {
    return _mqttClient != nullptr;
}

template<>
void MQTTManager::publish<bool>(const std::string& topic, const bool& value)
{

}

template<>
void MQTTManager::publish<int>(const std::string& topic, const int& value)
{

}

template<>
void MQTTManager::publish<float>(const std::string& topic, const float& value)
{

}

template<>
void MQTTManager::publish<std::string>(const std::string& topic, const std::string& value)
{

}

void MQTTManager::subscribe(const std::string& topic) {
    ESP_LOGI(TAG, "Subscribing to topic: %s (stub implementation)", topic.c_str());
    
    // TODO: Implement real MQTT subscription
    // if (_mqttClient) {
    //     int msg_id = esp_mqtt_client_subscribe(_mqttClient, topic.c_str(), 0);
    //     if (msg_id < 0) {
    //         ESP_LOGE(TAG, "Failed to subscribe to topic: %s", topic.c_str());
    //     } else {
    //         ESP_LOGI(TAG, "Subscription request sent for topic: %s (msg_id: %d)", topic.c_str(), msg_id);
    //     }
    // }
}

void MQTTManager::_mqttEventHandler(void* args, esp_event_base_t base, int32_t event_id, void* event_data) {
    auto* manager = static_cast<MQTTManager*>(args);
    auto* event = static_cast<esp_mqtt_event_handle_t>(event_data);
    
    switch (static_cast<esp_mqtt_event_id_t>(event_id)) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT client connected to broker");
            
            // Re-subscribe to all registered topics after reconnection
            for (const auto& subscription : manager->_subscriptions) {
                manager->subscribe(subscription.first);
            }
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT client disconnected from broker");
            break;
            
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "Successfully subscribed to topic (msg_id: %d)", event->msg_id);
            break;
            
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "Successfully unsubscribed from topic (msg_id: %d)", event->msg_id);
            break;
            
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGD(TAG, "Message published successfully (msg_id: %d)", event->msg_id);
            break;
            
        case MQTT_EVENT_DATA: {
            if (!event->topic || !event->data) {
                ESP_LOGW(TAG, "Received MQTT data with null topic or data");
                break;
            }
            
            std::string topic(event->topic, event->topic_len);
            std::string data(event->data, event->data_len);
            
            ESP_LOGI(TAG, "Received MQTT data - Topic: %s, Data: %s", topic.c_str(), data.c_str());
            
            // Find and call the corresponding callback
            auto it = manager->_subscriptions.find(topic);
            if (it != manager->_subscriptions.end()) {
                // Call the callback function
                it->second(data);
                ESP_LOGD(TAG, "Successfully processed data for topic: %s", topic.c_str());
            } else {
                ESP_LOGW(TAG, "No handler found for topic: %s", topic.c_str());
            }
            break;
        }
        
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT error occurred");
            if (event->error_handle) {
                if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                    ESP_LOGE(TAG, "TCP transport error: 0x%x", event->error_handle->esp_transport_sock_errno);
                } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
                    ESP_LOGE(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
                }
            }
            break;
            
        default:
            ESP_LOGD(TAG, "Unhandled MQTT event: %d", event_id);
            break;
    }
}
