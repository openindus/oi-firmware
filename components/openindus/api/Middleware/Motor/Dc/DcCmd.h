/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperCmd.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Slave.h"
#include "Master.h"
#include "Motor.h"

#if defined(CONFIG_MODULE_MASTER)

class DcCmd
{

public:

    DcCmd(ModuleControl* module);
    ~DcCmd() { 
        if (_currentEvent) vQueueDelete(_currentEvent);
        if (_callbackRegistered) Master::removeEventCallback(EVENT_MOTOR_DC_CURRENT, _module->getId());
    }
    
    /**
     * @brief Send a command to run the motor in the given direction
     * 
     * @param motor 
     * @param direction
     * @param dutyCycle
     */
    void run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle);

    /**
     * @brief Send a command to stop the motor
     * 
     * @param motor 
     * @param direction
     */
    void stop(MotorNum_t motor);

    /**
     * @brief Brake the DC motor (force brake mode)
     *
     * @param motor Motor number
     */
    void brake(MotorNum_t motor);

    /**
     * @brief Get the current consumption of the motor
     * 
     * @param motor Motor number
     * @return float Current in Amperes
     */
    float getCurrent(MotorNum_t motor);

    /**
     * @brief Get the DRV8873 fault status for a motor
     *
     * @param motor Motor number
     * @return uint8_t fault status (0 if none)
     */
    uint8_t getFault(MotorNum_t motor);

    /**
     * @brief Clear any DRV8873 fault for a motor
     *
     * @param motor Motor number
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t clearFault(MotorNum_t motor);

private:

    ModuleControl* _module;
    QueueHandle_t _currentEvent;
    bool _callbackRegistered;

};

#endif