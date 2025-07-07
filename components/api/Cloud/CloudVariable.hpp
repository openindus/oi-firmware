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
 * @brief Cloud variable update methods
 */
enum class UpdateMethod {
    SYNCHRONOUS,    // Synchronous update - once every refresh interval
    ASYNCHRONOUS    // Asynchronous update - on value change
};

/**
 * @brief MQTT update types
 */
enum class UpdateType {
    PUBLISH,    // Module publishes to MQTT topic
    SUBSCRIBE,  // Module subscribes to MQTT topic
    BOTH        // Module both publishes and subscribes
};

/**
 * @brief Cloud variable with MQTT support
 * @tparam T Data type (bool, int, float, std::string)
 */
template <typename T>
class CloudVariable {
private:
    std::string _name;             // Variable name and MQTT topic
    T _value;                      // Current value
    UpdateMethod _updateMethod;    // Update method
    UpdateType _updateType;        // Update type
    uint32_t _refreshInterval;     // Refresh interval (ms)
    uint32_t _lastUpdateTime;      // Last update time
    
    // Callback for MQTT publishing
    std::function<void(const std::string&, const T&)> _publishCallback;

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
        , _publishCallback(nullptr)
    {
    }

    // Getters
    const std::string& getName() const { return _name; }
    const T& getValue() const { return _value; }
    UpdateMethod getUpdateMethod() const { return _updateMethod; }
    UpdateType getUpdateType() const { return _updateType; }

    // Setters
    void setUpdateMethod(UpdateMethod method) { _updateMethod = method; }
    void setUpdateType(UpdateType type) { _updateType = type; }
    void setRefreshInterval(uint32_t interval) { _refreshInterval = interval; }
    void setPublishCallback(std::function<void(const std::string&, const T&)> callback) {
        _publishCallback = callback;
    }

    /**
     * @brief Set a new value
     * @param newValue New value
     * @return true if value has changed
     */
    bool setValue(const T& newValue) {
        if (_value != newValue) {
            _value = newValue;
            
            // Immediate publish if asynchronous
            if (_updateMethod == UpdateMethod::ASYNCHRONOUS) {
                publish();
            }
            return true;
        }
        return false;
    }

    /**
     * @brief Update from external source (MQTT)
     * @param newValue New received value
     */
    void updateFromExternal(const T& newValue) {
        if (_updateType == UpdateType::SUBSCRIBE || _updateType == UpdateType::BOTH) {
            _value = newValue;
        }
    }

    /**
     * @brief Periodic update (call regularly)
     */
    void update(void) {
        uint32_t currentTime = getCurrentTime();
        
        if (_updateMethod == UpdateMethod::SYNCHRONOUS) {
            if (currentTime - _lastUpdateTime >= _refreshInterval) {
                publish();
                _lastUpdateTime = currentTime;
            }
        }
    }

    /**
     * @brief Publish value to MQTT
     */
    void publish(void) {
        if (_publishCallback && 
            (_updateType == UpdateType::PUBLISH || _updateType == UpdateType::BOTH)) {
            _publishCallback(_name, _value);
        }
    }

private:
    /**
     * @brief Get current time
     */
    uint32_t getCurrentTime(void) {
        return xTaskGetTickCount() * portTICK_PERIOD_MS;
    }
};

// Type aliases
using BoolVariable = CloudVariable<bool>;
using IntVariable = CloudVariable<int>;
using FloatVariable = CloudVariable<float>;
using StringVariable = CloudVariable<std::string>;