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

int MotorDcPidCtrlCmdHandler::init(void)
{
    int err = 0;

    err |= MotorDcCmdHandler::init();

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_MOVE_TO, [](std::vector<uint8_t>& data) {
        MotorNum_t motor     = static_cast<MotorNum_t>(data[1]);
        float*     position  = reinterpret_cast<float*>(&data[2]);
        MotorDcPidCtrl::moveTo(motor, *position);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_STOP, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDcPidCtrl::stop(motor);
        data.clear();
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_GET_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor    = static_cast<MotorNum_t>(data[1]);
        float      position = MotorDcPidCtrl::getPosition(motor);
        uint8_t*   ptr      = reinterpret_cast<uint8_t*>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_MOTOR_DC_PID_POSITION;
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_MOTOR_DC_PID_SET_PARAMS, [](std::vector<uint8_t>& data) {
        MotorNum_t              motor  = static_cast<MotorNum_t>(data[1]);
        pid_ctrl_parameter_f_t* params = reinterpret_cast<pid_ctrl_parameter_f_t*>(&data[2]);
        MotorDcPidCtrl::setPidParams(motor, params);
        data.clear();
    });

    return err;
}

#endif
