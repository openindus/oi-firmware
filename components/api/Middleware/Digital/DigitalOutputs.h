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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "pcal6524.h"
#include "AnalogInputs.h"

#define DOUT_SENSOR_ADC_NO_OF_SAMPLES           64U
#define DOUT_SENSOR_RESISTOR_SENSE_VALUE        1200
#define DOUT_SENSOR_COEFF_BELOW_1A              1900
#define DOUT_SENSOR_VOLTAGE_BELOW_1A_mV         0.65f
#define DOUT_SENSOR_COEFF_BELOW_1_5A            1850
#define DOUT_SENSOR_VOLTAGE_BELOW_1_5A_mV       1.0f
#define DOUT_SENSOR_COEFF_BELOW_2A              1800
#define DOUT_SENSOR_VOLTAGE_BELOW_2A_mV         1.33f
#define DOUT_SENSOR_COEFF_ABOVE_2A              1750

#define DOUT_PWM_MAX_FREQUENCY_HZ                  1000
#define DOUT_PWM_MIN_FREQUENCY_HZ                  50

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
} DigitalOutputNum_t;

typedef enum {
    DIGITAL_OUTPUT_GPIO = 0,
    DIGITAL_OUTPUT_IOEX
} DigitalOutputType_t;


class DigitalOutputs
{
public:

    static void init(const gpio_num_t *gpio, const AdcNumChannel_t *adc, int num);
    static void init(ioex_device_t **ioex, const ioex_num_t *ioex_num, const ioex_num_t *ioex_current, int num);
      
    /**
     * @brief Set an output at high or low level.
     * 
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    static void digitalWrite(DigitalOutputNum_t num, uint8_t level);

    /**
     * @brief Toggle a digital output
     * 
     * @param num DOUT to toggle
    **/
    static void digitalToggle(DigitalOutputNum_t num);

    /**
     * @brief Set a digital output into PWM mode
     * 
     * @param num DOUT to set
     * @param freq PWM frequency [50 - 1000 Hz]
    **/
    static void digitalModePWM(DigitalOutputNum_t num, uint32_t freq);

    /**
     * @brief Set the duty cycle value of PWM for a digital output
     * 
     * @param num DOUT to set
     * @param duty Duty cycle, resolution is 14 bits : [0-16.383]
    **/
    static void digitalSetPWM(DigitalOutputNum_t num, uint32_t duty);
 
    /**
     * @brief Get the current of a digital output
     * 
     * @param num DOUT to get
     * @return current in Ampere
    **/
    static float digitalGetCurrent(DigitalOutputNum_t num);

    /**
     * @brief Get the overcurrent status of a digital output
     * 
     * @param num DOUT to get
     * @return 1 if overcurrent, 0 if not
    **/
    static int digitalGetOverCurrentStatus(DigitalOutputNum_t num);

private:

    /* Type of DOUT (gpio or ioex) */
    static DigitalOutputType_t _type;

    /* Number of DOUT */
    static uint8_t _nb; 
    
    /* GPIO num for DOUT (can be initialized as esp gpio or ioexpander gpio)*/
    static gpio_num_t* _gpio_num;
    static ioex_num_t* _ioex_num;

    /* ADC Channel for analog current or ioexpander gpio for digital current (HIGH or LOW current)*/
    static AdcNumChannel_t* _adc_current;
    static ioex_num_t* _ioex_current;

    /* Store a local copy of the pointer to an initalized ioex_device_t */
    static ioex_device_t** _ioex;
    static esp_adc_cal_characteristics_t _adc1Characteristics;
    static esp_adc_cal_characteristics_t _adc2Characteristics;
    static uint8_t* _doutLevel;

    static void _setLevel(DigitalOutputNum_t num, uint8_t level);
    static int _getLevel(DigitalOutputNum_t num);
    static void _controlTask(void *pvParameters);

};