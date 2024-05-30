/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputs.h
 * @brief Functions for DOUT
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

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

typedef enum {
    DOUT_MODE_DIGITAL = 0,
    DOUT_MODE_PWM
} DOut_Mode_t;

class DigitalOutputs
{
protected:

    static int init(const gpio_num_t *gpio, const AdcNumChannel_t *adc, int num);

public:
      
    /**
     * @brief Set an output at high or low level.
     * 
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    static void digitalWrite(DOut_Num_t num, uint8_t level);

    /**
     * @brief Toggle a digital output
     * 
     * @param num DOUT to toggle
    **/
    static void toggleOutput(DOut_Num_t num);

    /**
     * @brief Configure output mode
     * 
     * @param num ouput number
     * @param mode ouput mode [DIGITAL, PWM]
     */
    static void outputMode(DOut_Num_t num, DOut_Mode_t mode);

    /**
     * @brief Set the frequency value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
    **/
    static void setPWMFrequency(DOut_Num_t num, uint32_t freq);

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param duty Duty cycle, resolution is 14 bits : [0-16.383]
    **/
    static void setPWMDutyCycle(DOut_Num_t num, uint32_t duty);

    /**
     * @brief Get the current of a digital output
     * 
     * @param num DOUT to get
     * @return current in Ampere
    **/
    static float getOutputCurrent(DOut_Num_t num);

    /**
     * @brief Get the overcurrent status of a digital output
     * 
     * @param num DOUT to get
     * @return 1 if overcurrent, 0 if not
    **/
    static int outputIsOvercurrent(DOut_Num_t num);

private:

    /* Mode of each output (digital or PWM) */
    static DOut_Mode_t* _mode;

    /* Number of DOUT */
    static uint8_t _nb; 
    
    /* GPIO num for DOUT */
    static gpio_num_t* _gpio;

    /* ADC Channel for ouput current */
    static AdcNumChannel_t* _adc_current;
    static esp_adc_cal_characteristics_t _adc1Characteristics;
    static esp_adc_cal_characteristics_t _adc2Characteristics;

    static uint8_t* _level;
    static SemaphoreHandle_t _mutex;

    static void _controlTask(void *pvParameters);

};