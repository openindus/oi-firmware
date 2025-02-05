/**
 * @file DigitalOutputsCmd.h
 * @brief Digital Outputs Commands
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "MasterController.h"
#include "DigitalInputs.h"
#include "DigitalOutputs.h"

class DigitalOutputsCmd
{
public:
    DigitalOutputsCmd(Controller *controllerInst) : _controllerInst(controllerInst) {}

    /**
     * @brief Set an output at high or low level.
     *
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    void digitalWrite(DOut_Num_t num, bool level);

    template <typename T1, typename T2> 
    inline void digitalWrite(T1 num, T2 level) {
        digitalWrite((DOut_Num_t)num, (bool)level);
    }

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
     * @param duty Duty cycle, resolution is 14 bits : [0-16.383]
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
    Controller *_controllerInst;
};

#endif