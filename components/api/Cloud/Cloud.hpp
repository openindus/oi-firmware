/**
 * @file Cloud.hpp
 * @brief Header file for the Cloud API component.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

// Forward declaration of CloudVariable
template <typename T> class CloudVariable;

// Include CloudVariable types
using BoolVariable = CloudVariable<bool>;
using IntVariable = CloudVariable<int>;
using FloatVariable = CloudVariable<float>;
using StringVariable = CloudVariable<std::string>;

/**
 * @brief Cloud API class
 * 
 * This class provides the interface for cloud communication.
 */
class Cloud {
private:
    std::string _platformToken;
    std::string _macAddress;

public:
    /**
     * @brief Constructor for Cloud API
     * @param platformToken Platform authentication token
     * @param macAddress MAC address (optional, if null will use system MAC)
     */
    Cloud(const char* platformToken, const char* macAddress = nullptr);
    ~Cloud();

    /**
     * @brief Initializes the Cloud API.
     * @param brokerUrl MQTT broker URL
     * @return true if initialization is successful, false otherwise.
     */
    bool init(const char* brokerUrl = nullptr);

    /**
     * @brief Connects to the cloud service via MQTT.
     * @return true if connection is successful, false otherwise.
     */
    bool connect(void);

    /**
     * @brief Disconnects from the cloud service.
     */
    void disconnect(void);

    /**
     * @brief Update cloud variables and process MQTT messages
     * This function should be called regularly in the main loop
     */
    void update(void);

    /**
     * @brief Checks if the device is connected to the cloud.
     * @return true if connected, false otherwise.
     */
    bool isConnected(void) const;
    
    /**
     * @brief Get the platform token
     * @return Platform token string
     */
    const std::string& getPlatformToken(void) const;
    
    /**
     * @brief Get the MAC address
     * @return MAC address string
     */
    const std::string& getMacAddress(void) const;
    
    /**
     * @brief Register a CloudVariable with the Cloud system
     * @tparam T CloudVariable type
     * @param variable Reference to CloudVariable
     */
    template<typename T>
    void registerVariable(CloudVariable<T>& variable);
    
    /**
     * @brief Unregister a CloudVariable from the Cloud system
     * @param topicName Topic name to unregister
     */
    void unregisterVariable(const std::string& topicName);
};