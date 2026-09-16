/**
 * @file CloudVariable.hpp
 * @brief Header file for the CloudVariable class.
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "esp_timer.h"

/**
 * @brief Update methods (how often the value is published).
 * - SYNCHRONOUS : published once every refresh interval.
 * - ASYNCHRONOUS: published on change, rate-limited between minRefreshInterval
 *                 and maxRefreshInterval.
 */
enum class UpdateMethod {
    SYNCHRONOUS,
    ASYNCHRONOUS
};

/**
 * @brief Update types (direction relative to the MQTT topic).
 * - PUBLISH  : the module publishes the topic.
 * - SUBSCRIBE: the module subscribes to the topic.
 * - BOTH     : the module both publishes and subscribes.
 */
enum class UpdateType {
    PUBLISH,
    SUBSCRIBE,
    BOTH
};

/**
 * @brief Return the number of milliseconds since boot.
 */
static inline uint32_t cloudMillis(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

/**
 * @brief Type-erased base class for cloud variables.
 *
 * Allows a heterogeneous collection of CloudVariable<T> to be stored and
 * serviced by the Cloud background task without knowing the concrete type.
 */
class ICloudVariable {
protected:
    std::string _name;          // Variable name and MQTT topic leaf
    UpdateMethod _updateMethod; // Update method
    UpdateType _updateType;     // Update type (direction)
    uint32_t _syncInterval;     // SYNCHRONOUS refresh interval (ms)
    uint32_t _minInterval;      // ASYNCHRONOUS minimum interval between publishes (ms)
    uint32_t _maxInterval;      // ASYNCHRONOUS maximum interval without publish (ms)
    uint32_t _lastPublishMs;    // Last time the value was published (ms)
    bool _dirty;                // Value changed since last publish (ASYNCHRONOUS)
    char _typePrefix;           // MQTT topic type prefix ('b','i','f','s' or 'd' for default)

    ICloudVariable(const std::string& name, char typePrefix, UpdateMethod method,
                   UpdateType type, uint32_t syncInterval, uint32_t minInterval,
                   uint32_t maxInterval)
        : _name(name)
        , _updateMethod(method)
        , _updateType(type)
        , _syncInterval(syncInterval)
        , _minInterval(minInterval)
        , _maxInterval(maxInterval)
        , _lastPublishMs(0)
        , _dirty(true) // publish once at startup
        , _typePrefix(typePrefix)
    {}

public:
    virtual ~ICloudVariable() {}

    const std::string& getName(void) const { return _name; }
    char getTypePrefix(void) const { return _typePrefix; }
    UpdateMethod getUpdateMethod(void) const { return _updateMethod; }
    UpdateType getUpdateType(void) const { return _updateType; }

    /** @brief Override the MQTT topic type prefix (used by default variables -> 'd'). */
    void setTypePrefix(char prefix) { _typePrefix = prefix; }

    /**
     * @brief Force the variable to be published on the next service pass, without
     * changing its value. Used to re-announce a variable on each new MQTT session.
     */
    void requestPublish(void) { _dirty = true; }

    bool isPublisher(void) const {
        return _updateType == UpdateType::PUBLISH || _updateType == UpdateType::BOTH;
    }
    bool isSubscriber(void) const {
        return _updateType == UpdateType::SUBSCRIBE || _updateType == UpdateType::BOTH;
    }

    /** @brief Serialize the current value to the plain-text MQTT payload. */
    virtual std::string serialize(void) const = 0;

    /** @brief Apply an inbound MQTT payload to the value and fire the callback. */
    virtual void applyPayload(const std::string& payload) = 0;

    /**
     * @brief Decide whether the variable should be published now.
     *
     * SYNCHRONOUS  : true once per _syncInterval.
     * ASYNCHRONOUS : true when the value changed and at least _minInterval elapsed,
     *                or when _maxInterval elapsed regardless of change.
     *                A _maxInterval of 0 disables the periodic heartbeat, so the
     *                variable is published only when its value actually changes.
     * When it returns true it also records the publish time and clears the dirty flag.
     */
    bool shouldPublish(uint32_t nowMs) {
        if (!isPublisher()) {
            return false;
        }

        bool publish = false;
        if (_updateMethod == UpdateMethod::SYNCHRONOUS) {
            if ((nowMs - _lastPublishMs) >= _syncInterval) {
                publish = true;
            }
        } else { // ASYNCHRONOUS
            uint32_t elapsed = nowMs - _lastPublishMs;
            if (_dirty && elapsed >= _minInterval) {
                publish = true;
            } else if (_maxInterval != 0 && elapsed >= _maxInterval) {
                publish = true;
            }
        }

        if (publish) {
            _lastPublishMs = nowMs;
            _dirty = false;
        }
        return publish;
    }
};

/**
 * @brief Return the MQTT topic type prefix for a given data type.
 */
template <typename T> char cloudTypePrefix();
template <> inline char cloudTypePrefix<bool>() { return 'b'; }
template <> inline char cloudTypePrefix<int>() { return 'i'; }
template <> inline char cloudTypePrefix<float>() { return 'f'; }
template <> inline char cloudTypePrefix<std::string>() { return 's'; }

/**
 * @brief CloudVariable class
 * @tparam T Data type (bool, int, float, std::string)
 */
template <typename T>
class CloudVariable : public ICloudVariable {
private:
    T _value;                          // Current value
    std::function<void(const T&)> _cb; // User callback on inbound value (SUBSCRIBE/BOTH)

public:
    /**
     * @brief CloudVariable constructor
     * @param name Variable name and MQTT topic
     * @param initialValue Initial value
     * @param updateMethod Update method (default: SYNCHRONOUS)
     * @param updateType Update type/direction (default: PUBLISH)
     * @param refreshInterval SYNCHRONOUS interval, or ASYNCHRONOUS min interval, in ms
     * @param maxRefreshInterval ASYNCHRONOUS max interval without publish, in ms
     */
    CloudVariable(const std::string& name,
                  const T& initialValue = T(),
                  UpdateMethod updateMethod = UpdateMethod::SYNCHRONOUS,
                  UpdateType updateType = UpdateType::PUBLISH,
                  uint32_t refreshInterval = 1000,
                  uint32_t maxRefreshInterval = 10000)
        : ICloudVariable(name, cloudTypePrefix<T>(), updateMethod, updateType,
                         refreshInterval, refreshInterval, maxRefreshInterval)
        , _value(initialValue)
        , _cb(nullptr)
    {}

    // Getters
    const T& getValue(void) const { return _value; }

    // Setters
    void setUpdateMethod(UpdateMethod method) { _updateMethod = method; }
    void setUpdateType(UpdateType type) { _updateType = type; }
    void setRefreshInterval(uint32_t interval) {
        _syncInterval = interval;
        _minInterval = interval;
    }
    void setMaxRefreshInterval(uint32_t interval) { _maxInterval = interval; }

    /**
     * @brief Register a callback fired whenever a new value is received from the cloud.
     */
    void onReceive(std::function<void(const T&)> cb) { _cb = cb; }

    /**
     * @brief Set a new value
     * @param newValue New value
     * @return true if the value changed
     */
    bool setValue(const T& newValue) {
        if (_value != newValue) {
            _value = newValue;
            _dirty = true;
            return true;
        }
        return false;
    }

    std::string serialize(void) const override;

    void applyPayload(const std::string& payload) override {
        _value = _deserialize(payload);
        if (_cb) {
            _cb(_value);
        }
    }

private:
    static T _deserialize(const std::string& payload);
};

/* ------------------------------------------------------------------------- */
/* Serialization (spec 3.3 - plain text)                                     */
/* ------------------------------------------------------------------------- */

template <> inline std::string CloudVariable<bool>::serialize(void) const {
    return _value ? "1" : "0";
}
template <> inline std::string CloudVariable<int>::serialize(void) const {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", _value);
    return std::string(buf);
}
template <> inline std::string CloudVariable<float>::serialize(void) const {
    char buf[32];
    snprintf(buf, sizeof(buf), "%g", _value);
    return std::string(buf);
}
template <> inline std::string CloudVariable<std::string>::serialize(void) const {
    return _value;
}

/* ------------------------------------------------------------------------- */
/* Deserialization                                                           */
/* ------------------------------------------------------------------------- */

template <> inline bool CloudVariable<bool>::_deserialize(const std::string& p) {
    return !(p.empty() || p == "0" || p == "false");
}
template <> inline int CloudVariable<int>::_deserialize(const std::string& p) {
    return (int)strtol(p.c_str(), nullptr, 10);
}
template <> inline float CloudVariable<float>::_deserialize(const std::string& p) {
    return strtof(p.c_str(), nullptr);
}
template <> inline std::string CloudVariable<std::string>::_deserialize(const std::string& p) {
    return p;
}
