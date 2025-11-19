/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcCmd.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */
#include "DcCmd.h"

#if defined(CONFIG_MODULE_MASTER)

DcCmd::DcCmd(ModuleControl* module) : _module(module), _callbackRegistered(false)
{
    // Create queue for event-based current reading
    _currentEvent = xQueueCreate(1, sizeof(uint8_t*));
}

void DcCmd::run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&dutyCycle);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void DcCmd::stop(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_STOP, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

float DcCmd::getCurrent(MotorNum_t motor)
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
        ESP_LOGE("DcCmd", "Timeout waiting for current event from module ID %d", _module->getId());
        ESP_ERROR_CHECK(ESP_ERR_TIMEOUT);
    }
    float* current = reinterpret_cast<float*>(&data[2]);
    return *current;
}

#endif