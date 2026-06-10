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

#include "MotorDcCmd.h"
#include "MotorDcPidCtrl.h"
#include "EncoderCmd.h"
#include "Slave.h"
#include "Master.h"
#include "ModuleControl.h"
#include "Motor.h"
#include "pid_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class MotorDcPidCtrlCmd : public MotorDcCmd
{
public:
    /**
     * @brief Constructor
     * @param module Pointer to the remote module control interface
     */
    MotorDcPidCtrlCmd(ModuleControl* module);

    ~MotorDcPidCtrlCmd() = default;

    /**
     * @brief Attach an encoder to a motor for position feedback.
     * @param motor   Motor number
     * @param encoder Pointer to an EncoderCmd instance
     */
    void attachEncoder(MotorNum_t motor, EncoderCmd* encoder);

    /**
     * @brief Detach the encoder from a motor and stop position control.
     * @param motor Motor number
     */
    void detachEncoder(MotorNum_t motor);

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

    /**
     * @brief Perform a homing sequence on the remote module.
     * @param type        Homing strategy (only SENSOR_STOP supported)
     * @param dinNum      DIN connected to the homing sensor
     * @param motor       Motor number to drive during homing
     * @param dutyCycle   Motor duty cycle during homing (0–100 %)
     * @param invertLogic Invert sensor logic (default false)
     * @param timeoutMs   Maximum time allowed for homing (ms, default 30000)
     */
    void homing(HomingType_e type, DinNum_t dinNum, MotorNum_t motor,
        float dutyCycle, bool invertLogic = false, uint32_t timeoutMs = 30000);

private:
    ModuleControl* _module;
    QueueHandle_t  _positionEvent;
    bool           _positionCallbackRegistered;
};

#endif
