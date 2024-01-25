/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutput.c
 * @brief Functions for DOUT
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalOutput.h"

static const char DOUT_TAG[] = "DigitalOutput";

gpio_num_t* DigitalOutput::_gpio;

void DigitalOutput::init(gpio_config_t* config, gpio_num_t* num)
{
    _gpio = num;

    /* Init DOUT */
    ESP_LOGI(DOUT_TAG, "Init DOUT");
    ESP_ERROR_CHECK(gpio_config(config));

    /* Init DOUT current */
    ESP_LOGI(DOUT_TAG, "Init DOUT current");
    /** @todo */
}

void DigitalOutput::digitalWrite(DigitalOutputNum_t stor, uint8_t level) 
{
    if (stor < DOUT_MAX) {
        gpio_set_level(_gpio[stor], level);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", stor+1);
    }
}

void DigitalOutput::digitalToggle(DigitalOutputNum_t stor)
{
    int level;
    if (stor < DOUT_MAX) {
        level = (gpio_get_level(_gpio[stor]) == 1 ? 0 : 1);
        gpio_set_level(_gpio[stor], level);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", stor+1);
    }
}

void DigitalOutput::analogWrite(DigitalOutputNum_t stor, uint8_t duty) 
{
    DigitalOutput::ledcSetup(stor, 50, LEDC_TIMER_8_BIT);
    DigitalOutput::ledcWrite(stor, duty);
}

void DigitalOutput::ledcSetup(DigitalOutputNum_t stor, uint32_t freq, ledc_timer_bit_t bit)
{
    if (stor < DOUT_MAX) {
        ledc_timer_config_t ledcTimer = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = bit,
            .timer_num = LEDC_TIMER_1,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledcTimer));

        ledc_channel_config_t ledcChannel = {
            .gpio_num           = _gpio[stor],
            .speed_mode         = LEDC_LOW_SPEED_MODE,
            .channel            = (ledc_channel_t)(LEDC_CHANNEL_0 + stor),
            .intr_type          = LEDC_INTR_DISABLE,
            .timer_sel          = LEDC_TIMER_1,
            .duty               = 0,
            .hpoint             = 0,
            .flags {
                .output_invert  = 0
            }
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledcChannel));
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", stor+1);
    }
}

void DigitalOutput::ledcWrite(DigitalOutputNum_t stor, uint32_t duty)
{
    if (stor < DOUT_MAX) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    } else {
        ESP_LOGE(DOUT_TAG, "Invalid DOUT_%d", stor+1);
    }
}
