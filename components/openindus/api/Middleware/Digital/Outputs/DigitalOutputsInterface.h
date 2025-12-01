/**
 * @file DigitalOutputsInterface.h
 * @brief Digital Outputs Interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Digital Outputs Numbers
 *
 */
typedef enum {
    DOUT_1 = 0,
    DOUT_2,
    DOUT_3,
    DOUT_4,
    DOUT_5,
    DOUT_6,
    DOUT_7,
    DOUT_8,
    DOUT_MAX
} DOut_Num_t;

/**
 * @brief Digital Outputs Mode
 *
 */
typedef enum { 
    DOUT_MODE_DIGITAL = 0, 
    DOUT_MODE_PWM 
} DOut_Mode_t;

/**
 * @brief Digital Outputs Interface class
 *
 */
class DigitalOutputsInterface
{
public:
    virtual ~DigitalOutputsInterface() {}

    /**
     * @brief Set an output at high or low level.
     *
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    virtual void digitalWrite(DOut_Num_t num, bool level) = 0;

    /**
     * @brief Toggle a digital output
     *
     * @param num DOUT to toggle
     */
    virtual void toggleOutput(DOut_Num_t num) = 0;

    /**
     * @brief Configure output mode
     *
     * @param num output number
     * @param mode output mode [DIGITAL, PWM]
     */
    virtual void outputMode(DOut_Num_t num, DOut_Mode_t mode) = 0;

    /**
     * @brief Set the frequency value of PWM for a digital output
     *
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
     */
    virtual void setPWMFrequency(DOut_Num_t num, uint32_t freq) = 0;

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     *
     * @param num DOUT to set
     * @param duty Duty cycle in percentage [0-100] 
     */
    virtual void setPWMDutyCycle(DOut_Num_t num, float duty) = 0;

    /**
     * @brief Get the current of a digital output
     *
     * @param num DOUT to get
     * @return current in Ampere
     */
    virtual float getOutputCurrent(DOut_Num_t num) = 0;

    /**
     * @brief Get the overcurrent status of a digital output
     *
     * @param num DOUT to get
     * @return 1 if overcurrent, 0 if not
     */
    virtual int outputIsOvercurrent(DOut_Num_t num) = 0;

    /**
     * @brief Set the Overcurrent Threshold
     *
     * @param threshold
     * @param totalThreshold
     */
    virtual void setOvercurrentThreshold(float threshold, float thresholdSum = 8.0f) = 0;

    /**
     * @brief Attach a callback function to be called when an overcurrent is detected
     * 
     * @param callback Function pointer to the callback function
     * @param arg argument for the handler
     */
    virtual void attachOvercurrentCallback(void (*callback)(void*), void *arg = NULL) = 0;

    /**
     * @brief Detach the overcurrent callback function
     */
    virtual void detachOvercurrentCallback(void) = 0;
};
