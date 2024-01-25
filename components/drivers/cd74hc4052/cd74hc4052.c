#include "cd74hc4052.h"

const char MUX_TAG[] = "cd74hc4052";

/**
 * @brief Initialization of the GPIOs of the mux
 * @retval None
*/
void CD74HC4052_init(gpio_num_t sel0, gpio_num_t sel1){
    gpio_set_direction(sel0, GPIO_MODE_OUTPUT);
    gpio_set_direction(sel1, GPIO_MODE_OUTPUT);
    ESP_LOGI(MUX_TAG, "The mux GPIOs are set");
}

/**
 * @brief Selection of the two ouputs of the multiplexer
 * @param[in] channel
 * @retval None
*/
void CD74HC4052_output_selection(gpio_num_t sel0, gpio_num_t sel1, cd74hc4052_mux_channel_t channel){
    gpio_set_level(sel0, channel & 0b01);
    gpio_set_level(sel1, (channel & 0b10) >> 1);
}