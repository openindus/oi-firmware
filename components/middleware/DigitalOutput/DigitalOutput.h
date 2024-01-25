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

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

typedef enum {
    DOUT_1 = 0,
    DOUT_2,
#if !defined(CONFIG_BRUSHLESS)
    DOUT_3,
    DOUT_4,
#if !defined(CONFIG_MIXED)
    DOUT_5,
    DOUT_6,
    DOUT_7,
    DOUT_8,
#if !defined(CONFIG_DISCRETE) && !defined(CONFIG_DISCRETE_VE)
    DOUT_9,
    DOUT_10,
#endif
#endif
#endif
    DOUT_MAX
} DigitalOutputNum_t;

class DigitalOutput
{
public:

    static void init(gpio_config_t* config, gpio_num_t* num);
    static void digitalWrite(DigitalOutputNum_t stor, uint8_t level);
    static void digitalToggle(DigitalOutputNum_t stor);
    static void analogWrite(DigitalOutputNum_t stor, uint8_t duty);
    static void ledcSetup(DigitalOutputNum_t stor, uint32_t freq, ledc_timer_bit_t bit);
    static void ledcWrite(DigitalOutputNum_t stor, uint32_t duty);

private:

    static gpio_num_t* _gpio;
};