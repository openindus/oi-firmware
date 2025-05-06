/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Led.cpp
 * @brief Functions to control a onewire led
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Led.h"

static const char TAG[] = "Led";

#define LEDONEWIRE_T0H_NS (300)
#define LEDONEWIRE_T0L_NS (900)
#define LEDONEWIRE_T1H_NS (900)
#define LEDONEWIRE_T1L_NS (300)
#define LEDONEWIRE_RESET_US (80)

static uint32_t ledonewire_t0h_ticks = 0;
static uint32_t ledonewire_t1h_ticks = 0;
static uint32_t ledonewire_t0l_ticks = 0;
static uint32_t ledonewire_t1l_ticks = 0;

Led::Control_t Led::_control;
TaskHandle_t Led::_taskHandle;
rmt_channel_t Led::_rmt_channel; // RMT channel used to drive led
uint8_t Led::_buffer[3]; // buffer for led color
gpio_num_t Led::_gpio_num;
rmt_channel_t Led::_channel; // RMT Channel

void Led::install(gpio_num_t gpio_num)
{
    if (_control.installed == false) {
        ESP_LOGV(TAG, "installing driver...");

        // Installing RMT driver
        rmt_config_t rmt_param = RMT_DEFAULT_CONFIG_TX(gpio_num, RMT_CHANNEL_0);

        // set counter clock to 40MHz
        rmt_param.clk_div = 2;

        ESP_ERROR_CHECK(rmt_config(&rmt_param));
        ESP_ERROR_CHECK(rmt_driver_install(RMT_CHANNEL_0, 0, 0));

        uint32_t counter_clk_hz = 0;
        rmt_get_counter_clock(RMT_CHANNEL_0, &counter_clk_hz);
        float ratio = (float)counter_clk_hz / 1e9;

        ledonewire_t0h_ticks = (uint32_t)(ratio * LEDONEWIRE_T0H_NS);
        ledonewire_t0l_ticks = (uint32_t)(ratio * LEDONEWIRE_T0L_NS);
        ledonewire_t1h_ticks = (uint32_t)(ratio * LEDONEWIRE_T1H_NS);
        ledonewire_t1l_ticks = (uint32_t)(ratio * LEDONEWIRE_T1L_NS);

        // set ledonewire to rmt adapter
        rmt_translator_init(RMT_CHANNEL_0, _rmt_adapter);

        // set default color
        _buffer[0] = 0;
        _buffer[1] = 0;
        _buffer[2] = 0;

        ESP_LOGV(TAG, "driver was successfully installed");

        _control.color = LED_BLUE;
        _control.period = 1000;
        _control.blink = true;
        _control.blinkStatus = true;
        _control.installed = true;
        _control.intensity = 20;
        ESP_LOGV(TAG, "creating led task");
        xTaskCreate(_task, "Led task", 1024, NULL, 10, &_taskHandle);

        /* CLI */
        _registerCLI();
    }
}

void Led::remove(void)
{
    if (_control.installed == true) {
        ESP_LOGV(TAG, "removing driver...");
        if (rmt_driver_uninstall(RMT_CHANNEL_0) != ESP_OK) {
            ESP_LOGE(TAG, "error to remove");
        }
        _control.installed = false;
    }
}

/**
 * @brief Turn the led on with the given color
 * 
 * @param[in] LedColor_t color
 *
 */
void Led::on(LedColor_t color)
{
    if (_control.installed == true) {
        ESP_LOGV(TAG, "led on color:0x%02x", color);
        _control.color = color;
        _control.blink = false;
        xTaskNotify(_taskHandle, 0, eNoAction);
    }
}

/**
 * @brief Turn the led off
 * 
 */
void Led::off(void)
{
    if (_control.installed == true)
    {
        ESP_LOGV(TAG, "led off color");
        _control.color = LED_NONE;
        _control.blink = false;
        xTaskNotify(_taskHandle, 0, eNoAction);
    }
}

/**
 * @brief Blink the led with given color and period
 * 
 * @param[in] LedColor_t color
 * @param[in] uint32_t period un millisecond
 */
void Led::blink(LedColor_t color, uint32_t period)
{
    if (_control.installed == true)
    {
        ESP_LOGV(TAG, "led blink color:0x%02x | period:%u", color, period);
        _control.color = color;
        _control.period = period;
        _control.blinkStatus = true;
        _control.blink = true;
        xTaskNotify(_taskHandle, 0, eNoAction);
    }
}

/**
 * @brief Resync the led state
 * 
 */
void Led::sync(void)
{
    if (_control.installed == true)
    {
        ESP_LOGV(TAG, "led sync");
        xTaskNotify(_taskHandle, 0, eNoAction);
    }
}

void Led::_task(void *pvParameters)
{
    uint8_t red, green, blue;

    while(1) {
        xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS((_control.period) / 2));

        if (_control.installed) {
            red = (_control.color & LED_RED_MASK) ? _control.intensity : 0;
            green = (_control.color & LED_GREEN_MASK) ? _control.intensity : 0;
            blue = (_control.color & LED_BLUE_MASK) ? _control.intensity : 0;

            if (_control.blink) {
                if (_control.blinkStatus) {
                    _buffer[0] = green;
                    _buffer[1] = red;
                    _buffer[2] = blue;
                    rmt_write_sample(RMT_CHANNEL_0, _buffer, 3, true);
                    rmt_wait_tx_done(RMT_CHANNEL_0, pdMS_TO_TICKS(5));
                    _control.blinkStatus = false;
                } else {
                    _buffer[0] = 0;
                    _buffer[1] = 0;
                    _buffer[2] = 0;
                    rmt_write_sample(RMT_CHANNEL_0, _buffer, 3, true);
                    rmt_wait_tx_done(RMT_CHANNEL_0, pdMS_TO_TICKS(5));
                    _control.blinkStatus = true;
                }
            } else {
                _buffer[0] = green;
                _buffer[1] = red;
                _buffer[2] = blue;
                rmt_write_sample(RMT_CHANNEL_0, _buffer, 3, true);
                rmt_wait_tx_done(RMT_CHANNEL_0, pdMS_TO_TICKS(5));
            }
        }
    }
}

/**
 * @brief Conver RGB data to RMT format.
 *
 * @note For ledonewire, R,G,B each contains 256 different choices (i.e. uint8_t)
 *
 * @param[in] src: source data, to converted to RMT format
 * @param[in] dest: place where to store the convert result
 * @param[in] src_size: size of source data
 * @param[in] wanted_num: number of RMT items that want to get
 * @param[out] translated_size: number of source data that got converted
 * @param[out] item_num: number of RMT items which are converted from source data
 */
void IRAM_ATTR Led::_rmt_adapter(const void *src, rmt_item32_t *dest, size_t src_size,
    size_t wanted_num, size_t *translated_size, size_t *item_num)
{
    if (src == NULL || dest == NULL) {
        *translated_size = 0;
        *item_num = 0;
        return;
    }
    const rmt_item32_t bit0 = {{{ledonewire_t0h_ticks, 1, ledonewire_t0l_ticks, 0}}}; //Logical 0
    const rmt_item32_t bit1 = {{{ledonewire_t1h_ticks, 1, ledonewire_t1l_ticks, 0}}}; //Logical 1
    size_t size = 0;
    size_t num = 0;
    uint8_t *psrc = (uint8_t *)src;
    rmt_item32_t *pdest = dest;
    while (size < src_size && num < wanted_num) {
        for (int i = 0; i < 8; i++) {
            // MSB first
            if (*psrc & (1 << (7 - i))) {
                pdest->val = bit1.val;
            } else {
                pdest->val = bit0.val;
            }
            num++;
            pdest++;
        }
        size++;
        psrc++;
    }
    *translated_size = size;
    *item_num = num;
}
