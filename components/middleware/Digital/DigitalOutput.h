/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutput.h
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

#define DOUT_SENSOR_ADC_NO_OF_SAMPLES           64U
#define DOUT_SENSOR_RESISTOR_SENSE_VALUE        1200
#define DOUT_SENSOR_COEFF_BELOW_1A              1900
#define DOUT_SENSOR_VOLTAGE_BELOW_1A_mV         0.65f
#define DOUT_SENSOR_COEFF_BELOW_1_5A            1850
#define DOUT_SENSOR_VOLTAGE_BELOW_1_5A_mV       1.0f
#define DOUT_SENSOR_COEFF_BELOW_2A              1800
#define DOUT_SENSOR_VOLTAGE_BELOW_2A_mV         1.33f
#define DOUT_SENSOR_COEFF_ABOVE_2A              1750

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

typedef struct {
    adc_unit_t adc_num;
    adc_channel_t channel;
} AdcNumChannel_t;

class DigitalOutput
{
public:

    DigitalOutput(const gpio_num_t *gpio, const AdcNumChannel_t *adc, int num);
    DigitalOutput(const gpio_num_t *gpio, const adc_channel_t *adc, int num);
    DigitalOutput(ioex_device_t **ioex, const ioex_num_t *ioex_num, const ioex_num_t *current_num, int num);
    ~DigitalOutput();

    void init();
    void setLevel(DigitalOutputNum_t dout, uint8_t level);
    void setAll(uint8_t level);
    int getLevel(DigitalOutputNum_t dout);

    void write(DigitalOutputNum_t dout, uint8_t level);
    void toggle(DigitalOutputNum_t dout);
    void blink(DigitalOutputNum_t dout);

    void modePWM(DigitalOutputNum_t dout, uint32_t freq, ledc_timer_bit_t bit);
    void setDutyCycle(DigitalOutputNum_t dout, uint32_t duty);

    float getCurrent(DigitalOutputNum_t dout);
    int getCurrentLevel(DigitalOutputNum_t dout);

private:

    /* Type of DOUT (gpio or ioex) */
    DigitalOutputType_t _type;

    /* Number of DOUT */
    uint8_t _num; 
    
    /* GPIO num for DOUT (can be initialized as esp gpio or ioexpander gpio)*/
    gpio_num_t* _gpio_num;
    ioex_num_t* _ioex_num;

    /* ADC Channel for analog current or ioexpander gpio for digital current (HIGH or LOW current)*/
    AdcNumChannel_t* _adc_current;
    ioex_num_t* _ioex_current;

    /* Stor a local copy of the pointer to an initilized ioex_device_t */
    ioex_device_t** _ioex;
    esp_adc_cal_characteristics_t _adc1Characteristics;
    esp_adc_cal_characteristics_t _adc2Characteristics;
    uint8_t* _doutLevel;

    static void _controlTask(void *pvParameters);

};