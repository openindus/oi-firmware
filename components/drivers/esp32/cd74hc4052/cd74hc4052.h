#pragma once

#include <driver/gpio.h>
#include "esp_err.h"
#include "esp_log.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    CD74HC4052_MUX_OUTPUT_CHANA = 0x0,
    CD74HC4052_MUX_OUTPUT_CHANB = 0x1,
    CD74HC4052_MUX_OUTPUT_CHANC = 0x2,
    CD74HC4052_MUX_OUTPUT_CHAND = 0x3
}cd74hc4052_mux_channel_t;

/* Custom OpenIndus functions */

void CD74HC4052_init(gpio_num_t sel0, gpio_num_t sel1);

void CD74HC4052_output_selection(gpio_num_t sel0, gpio_num_t sel1, cd74hc4052_mux_channel_t channel);

#ifdef __cplusplus
}
#endif