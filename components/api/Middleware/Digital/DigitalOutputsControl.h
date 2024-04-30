/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputsControl.h
 * @author AFL
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_MASTER)

#include "ModuleControl.h"
#include "DigitalInputs.h"

class DigitalOutputsControl
{
public:

    DigitalOutputsControl(ModuleControl* control): _control(control) {}

    /**
     * @brief Set an output at high or low level.
     * 
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    void digitalWrite(DigitalOutputNum_t num, uint8_t level);

    /**
     * @brief Toggle a digital output
     * 
     * @param num DOUT to toggle
    **/
    void digitalToggle(DigitalOutputNum_t num);

    /**
     * @brief Set a digital output into PWM mode
     * 
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
    **/
    void digitalModePWM(DigitalOutputNum_t num, uint32_t freq);

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param duty Duty cycle
    **/
    void digitalSetPWM(DigitalOutputNum_t num, uint32_t duty);
 
    /**
     * @brief Get the current of a digital output
     * 
     * @param num DOUT to get
     * @return current in Ampere
    **/
    float digitalGetCurrent(DigitalOutputNum_t num);

private:

    ModuleControl* _control;
};

#endif