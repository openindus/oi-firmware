/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusIO.h
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc_cal.h"

class BusIO
{
public:

    typedef struct {
        adc_channel_t adcChannelId;
        adc_bits_width_t adcWidthId;
        gpio_num_t gpioNumSync;
        gpio_num_t gpioNumPower;
    } Config_t;

    static void init(Config_t* config);
    static uint16_t readId(void);
    static void powerOn(void);
    static void powerOff(void);

private:

    static esp_adc_cal_characteristics_t* _adcCharsId;
    static Config_t* _config;

};
