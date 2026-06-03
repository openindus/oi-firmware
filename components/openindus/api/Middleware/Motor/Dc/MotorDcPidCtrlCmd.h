/**
 * @file MotorDcPidCtrlCmd.h
 * @brief MotorDcPidCtrlCmd class — master-side bus commands for DC PID position control
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

#if defined(CONFIG_MODULE_MASTER)

#include "Slave.h"
#include "Master.h"
#include "ModuleControl.h"
#include "Motor.h"
#include "pid_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class MotorDcPidCtrlCmd
{
public:
    /**
     * @brief Constructor
     * @param module Pointer to the remote module control interface
     */
    MotorDcPidCtrlCmd(ModuleControl* module);

    ~MotorDcPidCtrlCmd() {
        if (_positionEvent) vQueueDelete(_positionEvent);
        if (_positionCallbackRegistered) Master::removeEventCallback(EVENT_MOTOR_DC_PID_POSITION, _module->getId());
    }

    /**
     * @brief Start closed-loop position control toward an absolute target.
     * @param motor    Motor number
     * @param position Target position in encoder pulses
     */
    void moveTo(MotorNum_t motor, float position);

    /**
     * @brief Stop position control and brake the motor immediately.
     * @param motor Motor number
     */
    void stop(MotorNum_t motor);

    /**
     * @brief Get the current encoder position.
     * @param motor Motor number
     * @return float Current position in encoder pulses
     */
    float getPosition(MotorNum_t motor);

    /**
     * @brief Update PID parameters for a motor at runtime.
     * @param motor  Motor number
     * @param params Pointer to the new PID parameter structure
     */
    void setPidParams(MotorNum_t motor, const pid_ctrl_parameter_f_t* params);

private:
    ModuleControl* _module;
    QueueHandle_t  _positionEvent;
    bool           _positionCallbackRegistered;
};

#endif
