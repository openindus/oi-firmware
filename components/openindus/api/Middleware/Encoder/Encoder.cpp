/**
 * @file Encoder.cpp
 * @brief Encoder class implementation
 * @author kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Encoder.h"
#include "esp_err.h"

static const char TAG[] = "Encoder";

static int max_glitch_us = 1;

int Encoder::begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr)
{
    ESP_LOGI(TAG, "Encoder initialization");

    // Save the pulse per revolution
    _ppr = ppr;

    // Create PCNT unit
    pcnt_unit_config_t unit_config = {
        .low_limit = -_ppr,
        .high_limit = _ppr,
        .intr_priority = 0,
        .flags = {
            .accum_count = 0,
        }
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &_pcntUnit));

    // Configure channel
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = _dinGpioMap[A],
        .level_gpio_num = _dinGpioMap[B],
        .flags = {
            .invert_edge_input = 0,
            .invert_level_input = 0,
            .virt_edge_io_level = 0,
            .virt_level_io_level = 0,
            .io_loop_back = 0, // Deprecated, not used
        },
    };
    pcnt_channel_handle_t pcnt_channel = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(_pcntUnit, &chan_config, &pcnt_channel));

    // Set edge and level actions for the channel
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // Add watch points and register callbacks
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcntUnit, _ppr));
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(_pcntUnit, -_ppr));

    // Register event callbacks
    pcnt_event_callbacks_t cbs = {
        .on_reach = _pcntIsrHandler,
    };
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(_pcntUnit, &cbs, this));

    // Configure and enable the input filter
    if (max_glitch_us > 0) {
        pcnt_glitch_filter_config_t filter_config = {
            .max_glitch_ns = static_cast<uint32_t>(max_glitch_us * 1000),
        };
        ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(_pcntUnit, &filter_config));
    }

    // Clear counter and enable unit
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcntUnit));
    ESP_ERROR_CHECK(pcnt_unit_enable(_pcntUnit));
    ESP_ERROR_CHECK(pcnt_unit_start(_pcntUnit));

    // Create a task to calculate the speed of the encoder
    xTaskCreate(_speedTask, "speedTask", 2048, this, 10, NULL);

    return 0;
}

void Encoder::end(void) 
{
    ESP_LOGI(TAG, "Encoder end");
    if (_pcntUnit) {
        ESP_ERROR_CHECK(pcnt_unit_stop(_pcntUnit));
        ESP_ERROR_CHECK(pcnt_unit_disable(_pcntUnit));
        ESP_ERROR_CHECK(pcnt_del_unit(_pcntUnit));
        _pcntUnit = nullptr;
    }
}

void Encoder::reset(void)
{
    _revolutionCnt = 0;
    ESP_ERROR_CHECK(pcnt_unit_clear_count(_pcntUnit));
}

int Encoder::getRevolutions(void)
{
    return _revolutionCnt;
}

int Encoder::getPulses(void)
{
    int val = 0;
    pcnt_unit_get_count(_pcntUnit, &val);
    return (val + (_revolutionCnt * _ppr));
}

float Encoder::getAngle(void)
{
    int val = 0;
    pcnt_unit_get_count(_pcntUnit, &val);
    if (_ppr == 0)
        return NAN;
    return (float)(((float)val / (float)_ppr) * 2.0 * 3.141592); // 2 * PI
}

float Encoder::getSpeed(void)
{
    return _speed;
}

bool Encoder::_pcntIsrHandler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
    Encoder* encoder = (Encoder*)user_ctx;
    
    if (edata->watch_point_value == encoder->_ppr) {
        encoder->_revolutionCnt += 1;
    } else if (edata->watch_point_value == -encoder->_ppr) {
        encoder->_revolutionCnt -= 1;
    }
    
    return false; // No task needs to be woken up
}

void Encoder::_speedTask(void *pvParameter)
{
    Encoder* encoder = (Encoder*)pvParameter;
    int last_pulse_count = 0;
    int current_pulse_count = 0;
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t frequency = pdMS_TO_TICKS(100); // 100 ms

    while (true) {
        vTaskDelayUntil(&last_wake_time, frequency);

        pcnt_unit_get_count(encoder->_pcntUnit, &current_pulse_count);
        int pulse_difference = current_pulse_count - last_pulse_count;

        // Adjust for overflow/underflow
        if (pulse_difference > encoder->_ppr / 2) {
            pulse_difference -= encoder->_ppr;
        } else if (pulse_difference < -encoder->_ppr / 2) {
            pulse_difference += encoder->_ppr;
        }

        last_pulse_count = current_pulse_count;

        // Calculate speed in pulses per second
        float speed = (float)pulse_difference / (float)pdTICKS_TO_MS(frequency) * 1000.0;
        encoder->_speed = speed;
    }
}