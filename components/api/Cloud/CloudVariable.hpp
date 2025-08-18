/**
 * @file CloudVariable.hpp
 * @brief Header file for the CloudVariable class.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Update methods
 */
enum class UpdateMethod {
    SYNCHRONOUS,
    ASYNCHRONOUS
};

/**
 * @brief Update types
 */
enum class UpdateType {
    PUBLISH,
    SUBSCRIBE,
    BOTH
};

/**
 * @brief CloudVariable class
 * @tparam T Data type (bool, int, float, std::string)
 */
template <typename T>
class CloudVariable {
private:
    std::string _name;          // Variable name and MQTT topic
    T _value;                   // Current value
    UpdateMethod _updateMethod; // Update method
    UpdateType _updateType;     // Update type
    uint32_t _refreshInterval;  // Refresh interval (ms)
    uint32_t _lastUpdateTime;   // Last update time

public:
    /**
     * @brief CloudVariable constructor
     * @param name Variable name and MQTT topic
     * @param initialValue Initial value
     * @param updateMethod Update method (default: SYNCHRONOUS)
     * @param updateType Update type (default: PUBLISH)
     * @param refreshInterval Refresh interval in ms (default: 1000ms)
     */
    CloudVariable(const std::string& name, 
                 const T& initialValue,
                 UpdateMethod updateMethod = UpdateMethod::SYNCHRONOUS,
                 UpdateType updateType = UpdateType::PUBLISH,
                 uint32_t refreshInterval = 1000)
        : _name(name)
        , _value(initialValue)
        , _updateMethod(updateMethod)
        , _updateType(updateType)
        , _refreshInterval(refreshInterval)
        , _lastUpdateTime(0)
    {}

    // Getters
    const std::string& getName() const { return _name; }
    const T& getValue() const { return _value; }
    UpdateMethod getUpdateMethod() const { return _updateMethod; }
    UpdateType getUpdateType() const { return _updateType; }

    // Setters
    void setUpdateMethod(UpdateMethod method) { _updateMethod = method; }
    void setUpdateType(UpdateType type) { _updateType = type; }
    void setRefreshInterval(uint32_t interval) { _refreshInterval = interval; }

    /**
     * @brief Set a new value
     * @param newValue New value
     * @return true if value has changed
     */
    bool setValue(const T& newValue) {
        if (_value != newValue) {
            _value = newValue;
            return true;
        }
        return false;
    }
};