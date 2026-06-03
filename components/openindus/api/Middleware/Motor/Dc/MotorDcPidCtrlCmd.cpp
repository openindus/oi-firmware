/**
 * @file MotorDcPidCtrlCmd.cpp
 * @brief MotorDcPidCtrlCmd class implementation — master-side bus commands for DC PID position control
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrlCmd.h"

#if defined(CONFIG_MODULE_MASTER)

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char* TAG = "MotorDcPidCtrlCmd";

MotorDcPidCtrlCmd::MotorDcPidCtrlCmd(ModuleControl* module)
    : _module(module)
    , _positionEvent(nullptr)
    , _positionCallbackRegistered(false)
{
    _positionEvent = xQueueCreate(1, sizeof(uint8_t*));
}

void MotorDcPidCtrlCmd::moveTo(MotorNum_t motor, float position)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_MOVE_TO, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void MotorDcPidCtrlCmd::stop(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_STOP, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

float MotorDcPidCtrlCmd::getPosition(MotorNum_t motor)
{
    if (!_positionCallbackRegistered) {
        Master::addEventCallback(EVENT_MOTOR_DC_PID_POSITION, _module->getId(), [this](uint8_t* data) {
            xQueueSend(_positionEvent, &data, pdMS_TO_TICKS(100));
        });
        _positionCallbackRegistered = true;
    }

    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_GET_POSITION, (uint8_t)motor};
    _module->runCallback(msgBytes, false);

    uint8_t* data = nullptr;
    xQueueReset(_positionEvent);
    if (xQueueReceive(_positionEvent, &data, pdMS_TO_TICKS(500)) != pdPASS) {
        ESP_LOGE(TAG, "Timeout waiting for position event from module ID %d", _module->getId());
        ESP_ERROR_CHECK(ESP_ERR_TIMEOUT);
    }

    float* position = reinterpret_cast<float*>(&data[2]);
    return *position;
}

void MotorDcPidCtrlCmd::setPidParams(MotorNum_t motor, const pid_ctrl_parameter_f_t* params)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_PID_SET_PARAMS, (uint8_t)motor};
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(params);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(pid_ctrl_parameter_f_t));
    _module->runCallback(msgBytes);
}

#endif
