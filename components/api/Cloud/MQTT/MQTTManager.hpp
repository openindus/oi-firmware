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
 * This class manages the MQTT client for cloud communication.
 */
class MQTTManager {
private:
    esp_mqtt_client_handle_t _mqttClient;
    std::map<std::string, std::function<void(const std::string&)>> _subscriptions;
    
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
     * @brief Initialize MQTT client
     * @param brokerUrl MQTT broker URL
     * @return true if successful, false otherwise
     */
    bool init(const char* brokerUrl);
    
    /**
     * @brief Connect to MQTT broker
     * @return true if successful, false otherwise
     */
    bool connect(void);
    
    /**
     * @brief Disconnect and cleanup MQTT client
     */
    void disconnect(void);
    
    /**
     * @brief Check if MQTT client is connected
     * @return true if connected, false otherwise
     */
    bool isConnected(void) const;
    
    /**
     * @brief Publish a message to MQTT topic
     * @tparam T Value type
     * @param topic MQTT topic
     * @param value Value to publish
     */
    template<typename T>
    void publish(const std::string& topic, const T& value);
    
    /**
     * @brief Subscribe to an MQTT topic
     * @param topic MQTT topic to subscribe to
     */
    void subscribe(const std::string& topic);
};
