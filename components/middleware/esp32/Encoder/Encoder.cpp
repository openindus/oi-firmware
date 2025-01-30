/**
 * @file Encoder.cpp
 * @brief Encoder class implementation
 * @author kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Encoder.h"
#include "driver/pcnt.h"

static const char TAG[] = "Encoder";

// A flag to identify if pcnt isr service has been installed.
static bool is_pcnt_isr_service_installed = false;
static int couter = 0;
static int max_glitch_us = 1;

static void pcnt_overflow_handler(void *arg)
{
    uint32_t status = 0;
    pcnt_get_event_status(PCNT_UNIT_0, &status);

    if (status & PCNT_EVT_H_LIM) {
        couter++;
    } else if (status & PCNT_EVT_L_LIM) {
        couter--;
    }
}

int Encoder::begin(DIn_Num_t A, DIn_Num_t B, uint32_t ppr)
{
    ESP_LOGI(TAG, "Encoder initialization");

    // Configure channel 0
    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = GPIO_NUM_11,
        .ctrl_gpio_num = GPIO_NUM_12,
        .lctrl_mode = PCNT_MODE_DISABLE,
        .hctrl_mode = PCNT_MODE_REVERSE,
        .pos_mode = PCNT_COUNT_INC, // Count both rising and falling edges
        .neg_mode = PCNT_COUNT_DEC, // Count both rising and falling edges
        .counter_h_lim = 1024,
        .counter_l_lim = -1024,
        .unit = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0,
    };
    pcnt_unit_config(&pcnt_config);

    // PCNT pause and reset value
    pcnt_counter_pause(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_0);

    // register interrupt handler in a thread-safe way
    if (!is_pcnt_isr_service_installed) {
        ESP_ERROR_CHECK(pcnt_isr_service_install(0));
        // make sure pcnt isr service won't be installed more than one time
        is_pcnt_isr_service_installed = true;
    }

    pcnt_isr_handler_add(PCNT_UNIT_0, pcnt_overflow_handler, NULL);

    pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM);
    pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_L_LIM);

    /* Configure and enable the input filter */
    ESP_ERROR_CHECK(pcnt_set_filter_value(PCNT_UNIT_0, max_glitch_us * 80));
    if (max_glitch_us) {
        pcnt_filter_enable(PCNT_UNIT_0);
    } else {
        pcnt_filter_disable(PCNT_UNIT_0);
    }

    // Start the PCNT unit
    pcnt_counter_resume(PCNT_UNIT_0);

    return 0;
}

void Encoder::end(void)
{
    ESP_LOGI(TAG, "Encoder end");
}

float Encoder::getPosition(void)
{
    int16_t val = 0;
    pcnt_get_counter_value(PCNT_UNIT_0, &val);
    return (float)(val + (couter * 1024));
}

float Encoder::getSpeed(void)
{
    ESP_LOGI(TAG, "Get the speed of the motor");
    return 0.0;
}

float Encoder::getAngle(void)
{
    int16_t val = 0;
    pcnt_get_counter_value(PCNT_UNIT_0, &val);
    return (float)((val / 1024.0) * 2 * 3.141592); // 2 * PI
}