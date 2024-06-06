/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCtrlCmd_DOut.h
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

#include "BusCtrlCmd.h"
#include "DigitalInputs.h"
#include "DigitalOutputs.h"

class BusCtrlCmd_DOut
{
public:

    BusCtrlCmd_DOut(BusCtrlCmd* control): _control(control) {}

    /**
     * @brief Set an output at high or low level.
     * 
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    void digitalWrite(DOut_Num_t num, uint8_t level);

    /**
     * @brief Toggle a digital output
     * 
     * @param num DOUT to toggle
    **/
    void toggleOutput(DOut_Num_t num);

    /**
     * @brief Configure output mode
     * 
     * @param num ouput number
     * @param mode ouput mode [DIGITAL, PWM]
     */
    void outputMode(DOut_Num_t num, DOut_Mode_t mode);

    /**
     * @brief Set the frequency value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
    **/
    void setPWMFrequency(DOut_Num_t num, uint32_t freq);

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param duty Duty cycle
    **/
    void setPWMDutyCycle(DOut_Num_t num, uint32_t duty);
 
    /**
     * @brief Get the current of a digital output
     * 
     * @param num DOUT to get
     * @return current in Ampere
    **/
    float getOutputCurrent(DOut_Num_t num);

private:

    BusCtrlCmd* _control;
};

#endif