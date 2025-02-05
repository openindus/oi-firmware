/**
 * @file Multiplexer.cpp
 * @brief Multiplexer
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Multiplexer.h"
#include "esp_log.h"

static const char TAG[] = "Multiplexer";

int Multiplexer::init(void)
{
    int ret = 0;

    gpio_config_t config = {
        .pin_bit_mask = ((1ULL << _inputPins[0]) |
                        (1ULL << _inputPins[1]) |
                        (1ULL << _inputPins[2]) |
                        (1ULL << _outputPins[0]) |
                        (1ULL << _outputPins[1]) |
                        (1ULL << _outputPins[2])),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ret |= gpio_config(&config);

    return ret;
}

int Multiplexer::route(Mux_IO_t input, Mux_IO_t output)
{
    int ret = 0;

    if (input < 0 || input > MUX_IO_MAX || output < 0 || output > MUX_IO_MAX) {
        ESP_LOGE(TAG, "%s: invalid input/output range", __FUNCTION__);
        return -1; 
    }

    // Set the input control pins
    for(int i = 0; i < 3; i++) {
        ret |= gpio_set_level(_inputPins[i], (input >> i) & 1);
    }

    // Set the output control pins
    for(int i = 0; i < 3; i++) {
        ret |= gpio_set_level(_outputPins[i], (output >> i) & 1);
    }

    return ret;
}
