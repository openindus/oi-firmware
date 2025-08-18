/**
 * @file BusIO.h
 * @brief Bus Control
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

class BusIO
{
public:
    struct Config_s {
        adc_channel_t adcChannelId;
        gpio_num_t gpioNumSync;
        gpio_mode_t gpioModeSync;
        gpio_num_t gpioNumPower;
        adc_oneshot_unit_handle_t adcHandle;
        adc_cali_handle_t adcCaliHandle;
    };

    static int init(Config_s* config);
    static uint32_t readId(void);
    static void powerOn(void);
    static void powerOff(void);
    static uint8_t readSync(void);
    static void writeSync(uint8_t level);
    static void toggleSync(void);

private:
    static Config_s* _config;

};
