/**
 * @file MotorDcPidCtrlCmd.cpp
 * @brief MotorDcPidCtrlCmd class implementation — master-side bus commands for DC PID position control
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrlCmd.h"

#if defined(CONFIG_MODULE_MASTER)

#include <cmath>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char* TAG = "MotorDcPidCtrlCmd";

MotorDcPidCtrlCmd::MotorDcPidCtrlCmd(ModuleControl* module)
    : MotorDcCmd(module)
    , _module(module)
    , _positionEvent(nullptr)
    , _positionCallbackRegistered(false)
    , _homingEvent(nullptr)
    , _homingCallbackRegistered(false)
{
    _positionEvent = xQueueCreate(1, sizeof(float));
    _homingEvent = xQueueCreate(4, sizeof(uint16_t));
}

void MotorDcPidCtrlCmd::attachEncoder(MotorNum_t motor, EncoderCmd* encoder)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_ATTACH_ENCODER, (uint8_t)motor, (uint8_t)encoder->getIndex()};
    _module->runCallback(msgBytes);
}

void MotorDcPidCtrlCmd::detachEncoder(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_DETACH_ENCODER, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

void MotorDcPidCtrlCmd::moveTo(MotorNum_t motor, float position)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_MOVE_TO, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorDcPidCtrlCmd::stop(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_STOP, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

float MotorDcPidCtrlCmd::getPosition(MotorNum_t motor)
{
    if (!_positionCallbackRegistered) {
        Master::addEventCallback(EVENT_MOTOR_DC_PID_POSITION, _module->getId(), [this](uint8_t* data) {
            float position;
            memcpy(&position, &data[2], sizeof(position));
            xQueueSend(_positionEvent, &position, 0);
        });
        _positionCallbackRegistered = true;
    }

    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_GET_POSITION, (uint8_t)motor};
    xQueueReset(_positionEvent);
    _module->runCallback(msgBytes, false);

    float position;
    if (xQueueReceive(_positionEvent, &position, pdMS_TO_TICKS(500)) != pdPASS) {
        ESP_LOGE(TAG, "Timeout waiting for position event from module ID %d", _module->getId());
        return NAN;
    }

    return position;
}

void MotorDcPidCtrlCmd::setPidParams(MotorNum_t motor, const pid_ctrl_parameter_f_t* params)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_CTRL_SET_PARAMS, (uint8_t)motor};
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(params);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(pid_ctrl_parameter_f_t));
    _module->runCallback(msgBytes);
}

void MotorDcPidCtrlCmd::homing(HomingType_e type, DinNum_t dinNum, MotorNum_t motor,
    float dutyCycle, bool invertLogic, uint32_t timeoutMs)
{
    std::vector<uint8_t> msgBytes = {
        CALLBACK_MOTOR_DC_PID_CTRL_HOMING,
        (uint8_t)type,
        (uint8_t)dinNum,
        (uint8_t)motor
    };
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&dutyCycle);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    msgBytes.push_back((uint8_t)invertLogic);
    ptr = reinterpret_cast<uint8_t*>(&timeoutMs);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    if (!_homingCallbackRegistered) {
        Master::addEventCallback(EVENT_MOTOR_DC_PID_HOMING_DONE, _module->getId(), [this](uint8_t* data) {
            uint16_t result = (uint16_t)data[1] | ((uint16_t)data[2] << 8);
            xQueueSend(_homingEvent, &result, 0);
        });
        _homingCallbackRegistered = true;
    }

    xQueueReset(_homingEvent);
    _module->runCallback(msgBytes);
}

bool MotorDcPidCtrlCmd::waitHoming(MotorNum_t motor, uint32_t timeoutMs)
{
    uint16_t result;
    TickType_t remaining = pdMS_TO_TICKS(timeoutMs);
    TickType_t start = xTaskGetTickCount();

    while (xQueueReceive(_homingEvent, &result, remaining) == pdPASS) {
        if ((MotorNum_t)(result & 0xFF) == motor) {
            return (result >> 8) != 0;
        }
        TickType_t elapsed = xTaskGetTickCount() - start;
        if (elapsed >= pdMS_TO_TICKS(timeoutMs)) {
            break;
        }
        remaining = pdMS_TO_TICKS(timeoutMs) - elapsed;
    }

    ESP_LOGE(TAG, "Timeout waiting for homing event from module ID %d", _module->getId());
    return false;
}

#endif