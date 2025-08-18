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
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"
#include "Led.h"

static const char TAG[] = "Led";

#define LEDONEWIRE_T0H_NS (300)
#define LEDONEWIRE_T0L_NS (900)
#define LEDONEWIRE_T1H_NS (900)
#define LEDONEWIRE_T1L_NS (300)
#define LEDONEWIRE_RESET_US (80)

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_led_strip_encoder_t;

Led::Control_t Led::_control;
TaskHandle_t Led::_taskHandle;
rmt_channel_handle_t Led::_rmt_channel_handle = NULL;
rmt_encoder_handle_t Led::_led_encoder = NULL;
uint8_t Led::_buffer[3]; // buffer for led color
gpio_num_t Led::_gpio_num;

void Led::install(gpio_num_t gpio_num)
{
    if (_control.installed == false) {
        ESP_LOGV(TAG, "installing driver...");

        // Create RMT TX channel
        rmt_tx_channel_config_t tx_chan_config = {
            .gpio_num = gpio_num,
            .clk_src = RMT_CLK_SRC_DEFAULT,
            .resolution_hz = 10000000, // 10MHz, 1 tick = 100ns
            .mem_block_symbols = 48,
            .trans_queue_depth = 4,
            .intr_priority = 0, // Use default interrupt priority
            .flags = {
                .invert_out = 0,     // Don't invert output signal
                .with_dma = 0,       // Disable DMA for simple LED control
                .io_loop_back = 0,   // No loopback needed
                .io_od_mode = 0,     // Use push-pull mode
                .allow_pd = 0,       // Don't allow power domain shutdown
            },
        };
        ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &_rmt_channel_handle));

        // Create bytes encoder for WS2812 LED strip
        rmt_bytes_encoder_config_t bytes_encoder_config = {
            .bit0 = {
                .duration0 = 3, // T0H=0.3us
                .level0 = 1,
                .duration1 = 9, // T0L=0.9us
                .level1 = 0,
            },
            .bit1 = {
                .duration0 = 9, // T1H=0.9us
                .level0 = 1,
                .duration1 = 3, // T1L=0.3us
                .level1 = 0,
            },
            .flags = {
                .msb_first = 1, // WS2812 bit order is MSB first
            },
        };
        ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, &_led_encoder));

        // Enable the RMT TX channel
        ESP_ERROR_CHECK(rmt_enable(_rmt_channel_handle));

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
        
        // Disable and delete the RMT TX channel
        if (_rmt_channel_handle != NULL) {
            ESP_ERROR_CHECK(rmt_disable(_rmt_channel_handle));
            ESP_ERROR_CHECK(rmt_del_channel(_rmt_channel_handle));
            _rmt_channel_handle = NULL;
        }
        
        // Delete the LED encoder
        if (_led_encoder != NULL) {
            ESP_ERROR_CHECK(rmt_del_encoder(_led_encoder));
            _led_encoder = NULL;
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
    rmt_transmit_config_t tx_config = {
        .loop_count = 0, // no loop
        .flags = {
            .eot_level = 0,         // End of transmission level low
            .queue_nonblocking = 0, // Use blocking queue behavior
        },
    };

    while(1) {
        xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS((_control.period) / 2));

        if (_control.installed && _rmt_channel_handle != NULL && _led_encoder != NULL) {
            red = (_control.color & LED_RED_MASK) ? _control.intensity : 0;
            green = (_control.color & LED_GREEN_MASK) ? _control.intensity : 0;
            blue = (_control.color & LED_BLUE_MASK) ? _control.intensity : 0;

            if (_control.blink) {
                if (_control.blinkStatus) {
                    _buffer[0] = green;
                    _buffer[1] = red;
                    _buffer[2] = blue;
                    ESP_ERROR_CHECK(rmt_transmit(_rmt_channel_handle, _led_encoder, _buffer, sizeof(_buffer), &tx_config));
                    ESP_ERROR_CHECK(rmt_tx_wait_all_done(_rmt_channel_handle, pdMS_TO_TICKS(5)));
                    _control.blinkStatus = false;
                } else {
                    _buffer[0] = 0;
                    _buffer[1] = 0;
                    _buffer[2] = 0;
                    ESP_ERROR_CHECK(rmt_transmit(_rmt_channel_handle, _led_encoder, _buffer, sizeof(_buffer), &tx_config));
                    ESP_ERROR_CHECK(rmt_tx_wait_all_done(_rmt_channel_handle, pdMS_TO_TICKS(5)));
                    _control.blinkStatus = true;
                }
            } else {
                _buffer[0] = green;
                _buffer[1] = red;
                _buffer[2] = blue;
                ESP_ERROR_CHECK(rmt_transmit(_rmt_channel_handle, _led_encoder, _buffer, sizeof(_buffer), &tx_config));
                ESP_ERROR_CHECK(rmt_tx_wait_all_done(_rmt_channel_handle, pdMS_TO_TICKS(5)));
            }
        }
    }
}

