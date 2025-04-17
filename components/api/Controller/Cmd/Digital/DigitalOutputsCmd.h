/**
 * @file DigitalOutputsCmd.h
 * @brief Digital Outputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "DigitalInputs.h"
#include "DigitalOutputs.h"
#include "Master.h"
#include "Slave.h"

class DigitalOutputsCmd
{
public:
    DigitalOutputsCmd(Controller *controller) : _controller(controller) {}

    /**
     * @brief Set an output at high or low level.
     *
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    void digitalWrite(DOut_Num_t num, bool level);
    
    template <typename T1, typename T2> inline void digitalWrite(T1 num, T2 level) {
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
     * @param duty Duty cycle in percentage [0-100]
     **/
    void setPWMDutyCycle(DOut_Num_t num, float duty);

    /**
     * @brief Get the current of a digital output
     *
     * @param num DOUT to get
     * @return current in Ampere
     **/
    float getOutputCurrent(DOut_Num_t num);

    /**
     * @brief Set the Overcurrent Threshold
     *
     * @param threshold
     * @param totalThreshold
     */
    void setOvercurrentThreshold(float threshold, float thresholdSum = 8.0f);

    /**
     * @brief Attach a callback function to be called when an overcurrent is detected
     *
     * @param callback Function pointer to the callback function
     */
    void attachOvercurrentCallback(void (*callback)(void));

    /**
     * @brief Detach the overcurrent callback function
     */
    void detachOvercurrentCallback(void);

private:
    Controller *_controller;

    void (*_overcurrentCallback)(void) = NULL;
};

#endif