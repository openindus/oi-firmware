/**
 * @file MQTTManager.cpp
 * @brief Implementation file for MQTTManager class
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MQTTManager.hpp"

// The Cloud library is only availableon core module
// (a slave has no network stack of its own and is driven over the bus).
#if defined(CONFIG_OI_CORE)

#include "esp_crt_bundle.h"

static const char* TAG = "MQTTManager";

// Static instance initialization
MQTTManager* MQTTManager::_instance = nullptr;

MQTTManager::MQTTManager() : _mqttClient(nullptr), _connected(false) {
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

bool MQTTManager::init(const char* brokerUri, const char* username, const char* password) {
    if (_mqttClient) {
        ESP_LOGW(TAG, "MQTT client already initialized");
        return true;
    }

    if (!brokerUri) {
        ESP_LOGE(TAG, "Broker URI is null");
        return false;
    }

    ESP_LOGI(TAG, "Initializing MQTT client with broker: %s", brokerUri);

    // Configure MQTT client (MQTT over WebSocket / WSS)
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = brokerUri;
    mqtt_cfg.session.keepalive = 60;
    mqtt_cfg.session.disable_clean_session = false;
    mqtt_cfg.network.disable_auto_reconnect = false;
    if (username) {
        mqtt_cfg.credentials.username = username;
    }
    if (password) {
        mqtt_cfg.credentials.authentication.password = password;
    }
    // Server verification for wss:// via the ESP-IDF certificate bundle
    mqtt_cfg.broker.verification.crt_bundle_attach = esp_crt_bundle_attach;

    _mqttClient = esp_mqtt_client_init(&mqtt_cfg);
    if (!_mqttClient) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return false;
    }

    esp_err_t err = esp_mqtt_client_register_event(
        _mqttClient, MQTT_EVENT_ANY, _mqttEventHandler, this);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register MQTT event handler: %s", esp_err_to_name(err));
        esp_mqtt_client_destroy(_mqttClient);
        _mqttClient = nullptr;
        return false;
    }

    ESP_LOGI(TAG, "MQTT client initialization completed");
    return true;
}

bool MQTTManager::connect(void) {
    if (!_mqttClient) {
        ESP_LOGE(TAG, "MQTT client not initialized. Call init() first");
        return false;
    }

    ESP_LOGI(TAG, "Starting MQTT client");

    esp_err_t result = esp_mqtt_client_start(_mqttClient);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(result));
        return false;
    }

    ESP_LOGI(TAG, "MQTT client connection initiated");
    return true;
}

void MQTTManager::disconnect(void) {
    if (_mqttClient) {
        ESP_LOGI(TAG, "Disconnecting MQTT client");
        esp_mqtt_client_stop(_mqttClient);
        esp_mqtt_client_destroy(_mqttClient);
        _mqttClient = nullptr;
        _connected = false;
        _subscriptions.clear();
    }
}

bool MQTTManager::isConnected(void) const {
    return _connected;
}

int MQTTManager::publish(const std::string& topic, const std::string& payload, int qos, bool retain) {
    if (!_mqttClient || !_connected) {
        ESP_LOGW(TAG, "Cannot publish, MQTT not connected");
        return -1;
    }

    int msg_id = esp_mqtt_client_publish(_mqttClient, topic.c_str(),
                                         payload.c_str(), payload.length(), qos, retain);
    if (msg_id < 0) {
        ESP_LOGE(TAG, "Failed to publish to topic: %s", topic.c_str());
    } else {
        ESP_LOGD(TAG, "Published to %s: %s (msg_id: %d)", topic.c_str(), payload.c_str(), msg_id);
    }
    return msg_id;
}

void MQTTManager::subscribe(const std::string& topic, MessageCallback callback) {
    // Register the callback so inbound data can be dispatched (and re-subscribed on reconnect)
    _subscriptions[topic] = callback;

    if (_mqttClient && _connected) {
        int msg_id = esp_mqtt_client_subscribe(_mqttClient, topic.c_str(), 0);
        if (msg_id < 0) {
            ESP_LOGE(TAG, "Failed to subscribe to topic: %s", topic.c_str());
        } else {
            ESP_LOGI(TAG, "Subscription request sent for topic: %s (msg_id: %d)", topic.c_str(), msg_id);
        }
    } else {
        ESP_LOGD(TAG, "Deferred subscription to %s (not connected yet)", topic.c_str());
    }
}

void MQTTManager::_mqttEventHandler(void* args, esp_event_base_t base, int32_t event_id, void* event_data) {
    auto* manager = static_cast<MQTTManager*>(args);
    auto* event = static_cast<esp_mqtt_event_handle_t>(event_data);

    switch (static_cast<esp_mqtt_event_id_t>(event_id)) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT client connected to broker");
            manager->_connected = true;

            // (Re-)subscribe to all registered topics after (re)connection
            for (const auto& subscription : manager->_subscriptions) {
                int msg_id = esp_mqtt_client_subscribe(manager->_mqttClient,
                                                       subscription.first.c_str(), 0);
                ESP_LOGI(TAG, "Re-subscribing to %s (msg_id: %d)",
                         subscription.first.c_str(), msg_id);
            }
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT client disconnected from broker");
            manager->_connected = false;
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
            ESP_LOGD(TAG, "Unhandled MQTT event: %d", (int)event_id);
            break;
    }
}

#endif // CONFIG_OI_CORE
