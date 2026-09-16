/**
 * @file MotorDcPidCtrlCmdHandler.cpp
 * @brief MotorDcPidCtrlCmdHandler class implementation — slave-side bus callback handler for DC PID position control
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrlCmdHandler.h"

#if defined(CONFIG_MODULE_SLAVE)

#include "esp_log.h"

static const char* TAG = "MotorDcPidCtrlCmdHandler";

Encoder** MotorDcPidCtrlCmdHandler::_encoders = nullptr;

struct HomingRequest_t {
    HomingType_e type;
    DinNum_t dinNum;
    MotorNum_t motor;
    float dutyCycle;
    bool invertLogic;
    uint32_t timeoutMs;
};

static void homingTask(void* arg)
{
    HomingRequest_t* request = static_cast<HomingRequest_t*>(arg);
    bool success = MotorDcPidCtrl::homing(request->type, request->dinNum, request->motor,
                                          request->dutyCycle, request->invertLogic, request->timeoutMs);
    Slave::sendEvent({EVENT_MOTOR_DC_PID_HOMING_DONE, (uint8_t)request->motor, (uint8_t)success});
    delete request;
    vTaskDelete(nullptr);
}

int MotorDcPidCtrlCmdHandler::init(Encoder** encoders)
{
    int err = 0;

    err |= MotorDcCmdHandler::init();

    _encoders = encoders;

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_ATTACH_ENCODER, [](std::vector<uint8_t>& data) {
        MotorNum_t motor        = static_cast<MotorNum_t>(data[1]);
        uint8_t    encoderIndex = data[2];
        if (_encoders != nullptr) {
            MotorDcPidCtrl::attachEncoder(motor, _encoders[encoderIndex]);
        } else {
            ESP_LOGE(TAG, "No encoder array registered");
        }
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_DETACH_ENCODER, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDcPidCtrl::detachEncoder(motor);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_MOVE_TO, [](std::vector<uint8_t>& data) {
        MotorNum_t motor     = static_cast<MotorNum_t>(data[1]);
        float*     position  = reinterpret_cast<float*>(&data[2]);
        MotorDcPidCtrl::moveTo(motor, *position);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_STOP, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDcPidCtrl::stop(motor);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_GET_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor    = static_cast<MotorNum_t>(data[1]);
        float      position = MotorDcPidCtrl::getPosition(motor);
        uint8_t*   ptr      = reinterpret_cast<uint8_t*>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_MOTOR_DC_PID_POSITION;
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_SET_PARAMS, [](std::vector<uint8_t>& data) {
        MotorNum_t              motor  = static_cast<MotorNum_t>(data[1]);
        pid_ctrl_parameter_f_t* params = reinterpret_cast<pid_ctrl_parameter_f_t*>(&data[2]);
        MotorDcPidCtrl::setPidParams(motor, params);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_CTRL_HOMING, [](std::vector<uint8_t>& data) {
        HomingRequest_t* request = new HomingRequest_t;
        request->type = static_cast<HomingType_e>(data[1]);
        request->dinNum = static_cast<DinNum_t>(data[2]);
        request->motor = static_cast<MotorNum_t>(data[3]);
        memcpy(&request->dutyCycle, &data[4], sizeof(request->dutyCycle));
        request->invertLogic = (bool)data[8];
        memcpy(&request->timeoutMs, &data[9], sizeof(request->timeoutMs));
        if (xTaskCreate(homingTask, "dc_homing", 4096, request, 5, nullptr) != pdPASS) {
            ESP_LOGE(TAG, "Failed to create homing task");
            Slave::sendEvent({EVENT_MOTOR_DC_PID_HOMING_DONE, (uint8_t)request->motor, 0});
            delete request;
        }
        data.clear();
    });

    return err;
}

#endif