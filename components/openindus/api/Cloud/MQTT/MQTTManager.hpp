/**
 * @file MQTTManager.hpp
 * @brief Header file for MQTTManager class
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "mqtt_client.h"

/**
 * @brief MQTTManager class
 *
 * Singleton wrapping the ESP-IDF esp-mqtt client. Configured for MQTT over
 * (secure) WebSocket. Holds a topic -> callback map used to dispatch inbound
 * messages, re-subscribed automatically on reconnection.
 */
class MQTTManager {
public:
    using MessageCallback = std::function<void(const std::string&)>;

private:
    esp_mqtt_client_handle_t _mqttClient;
    std::map<std::string, MessageCallback> _subscriptions;
    volatile bool _connected;

    static MQTTManager* _instance;

    // Private constructor for singleton
    MQTTManager();

    // Static MQTT event handler
    static void _mqttEventHandler(void* args, esp_event_base_t base, int32_t event_id, void* event_data);

public:
    // Singleton access
    static MQTTManager* getInstance();

    // Destructor
    ~MQTTManager();

    /**
     * @brief Initialize the MQTT client (MQTT over WebSocket / WSS).
     * @param brokerUri Broker URI, e.g. "wss://host/mqtt"
     * @param username Client username (device UUID)
     * @param password Client password (device token)
     * @return true if successful, false otherwise
     */
    bool init(const char* brokerUri, const char* username, const char* password);

    /**
     * @brief Start the MQTT client (asynchronous connect).
     * @return true if the start request succeeded, false otherwise
     */
    bool connect(void);

    /**
     * @brief Stop and destroy the MQTT client.
     */
    void disconnect(void);

    /**
     * @brief Check if the MQTT client is connected to the broker.
     * @return true if connected, false otherwise
     */
    bool isConnected(void) const;

    /**
     * @brief Publish a plain-text payload to an MQTT topic.
     * @param topic MQTT topic
     * @param payload Plain-text payload
     * @param qos Quality of service (default 0)
     * @param retain Retain flag (default false)
     * @return message id on success, -1 on failure
     */
    int publish(const std::string& topic, const std::string& payload, int qos = 0, bool retain = false);

    /**
     * @brief Subscribe to an MQTT topic and register a callback for inbound data.
     * @param topic MQTT topic to subscribe to
     * @param callback Callback invoked with the payload when data is received
     */
    void subscribe(const std::string& topic, MessageCallback callback);
};
