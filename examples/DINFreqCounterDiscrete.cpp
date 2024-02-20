/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_timer.h"

pcnt_unit_handle_t flow_pcnt_unit =  NULL;
int64_t flow_timer;

float get_frequency(void) {
    
    // get elapse time
    int64_t new_time = esp_timer_get_time();
    // get pulse count
    int pulse_count;
    pcnt_unit_get_count(flow_pcnt_unit, &pulse_count);
    // calculate frequency
    float freq = ((float) pulse_count)/((float)(new_time-flow_timer)/1000000);

    // get new time just before reset
    flow_timer = esp_timer_get_time();
    pcnt_unit_clear_count(flow_pcnt_unit);

    return freq;
}

void app_main(void)
{
    printf("Hello world!\n");

    pcnt_unit_config_t flow_pcnt_config = {
        .high_limit = 32767,
        .low_limit = -1,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_unit(&flow_pcnt_config, &flow_pcnt_unit));

    // Set filter
    pcnt_glitch_filter_config_t flow_filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_set_glitch_filter(flow_pcnt_unit, &flow_filter_config));

    // Configure edge cound
    pcnt_chan_config_t flow_chan_config = {
        .edge_gpio_num = GPIO_NUM_20,
        .level_gpio_num = -1,
    };
    pcnt_channel_handle_t flow_pcnt_chan = NULL;
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_new_channel(flow_pcnt_unit, &flow_chan_config, &flow_pcnt_chan));

    // Count only positive edge
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_channel_set_edge_action(flow_pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_enable(flow_pcnt_unit));
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_clear_count(flow_pcnt_unit));

    // get time before start
    flow_timer = esp_timer_get_time();

    ESP_ERROR_CHECK_WITHOUT_ABORT(pcnt_unit_start(flow_pcnt_unit));

    while(1)
    {
        printf("Freq: %fHz\n", get_frequency());
        vTaskDelay(50);
    }
}
