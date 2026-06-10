/**
 * @file MotorDcCmd.cpp
 * @brief MotorDcCmd class implementation
 * @author OpenIndus Team
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcCmd.h"

#if defined(CONFIG_MODULE_MASTER)

static const char* TAG = "MotorDcCmd";

MotorDcCmd::MotorDcCmd(ModuleControl* module) : _module(module), _callbackRegistered(false)
{
    // Create queue for event-based current reading
    _currentEvent = xQueueCreate(1, sizeof(uint8_t*));
}

void MotorDcCmd::run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&dutyCycle);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorDcCmd::stop(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_STOP, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

void MotorDcCmd::brake(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_BRAKE, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

float MotorDcCmd::getCurrent(MotorNum_t motor)
{
    // Register event callback on first use to avoid static initialization order issues
    if (!_callbackRegistered) {
        Master::addEventCallback(EVENT_MOTOR_DC_CURRENT, _module->getId(), [this](uint8_t* data) {
            xQueueSend(_currentEvent, &data, pdMS_TO_TICKS(100));
        });
        _callbackRegistered = true;
    }
    
    // Send a message to slave to request current read but do not wait for response
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_GET_CURRENT, (uint8_t)motor};
    _module->runCallback(msgBytes, false);

    // Wait for event from slave
    uint8_t* data = NULL;
    xQueueReset(_currentEvent);
    if (xQueueReceive(_currentEvent, &data, pdMS_TO_TICKS(500)) != pdPASS) {
        // Crash or handle timeout error
        ESP_LOGE(TAG, "Timeout waiting for current event from module ID %d", _module->getId());
        ESP_ERROR_CHECK(ESP_ERR_TIMEOUT);
    }
    float* current = reinterpret_cast<float*>(&data[2]);
    return *current;
}

uint8_t MotorDcCmd::getFault(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_GET_FAULT, (uint8_t)motor};
    int ret = _module->runCallback(msgBytes);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to get fault status from module %d", _module->getId());
        return 0xFF; // indicate error
    }
    if (msgBytes.size() >= 3) {
        return msgBytes[2];
    }
    return 0x00; // No fault
}

esp_err_t MotorDcCmd::clearFault(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_CLEAR_FAULT, (uint8_t)motor};
    int ret = _module->runCallback(msgBytes);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to clear fault on module %d", _module->getId());
        return ESP_FAIL;
    }
    return ESP_OK;
}

#endif
