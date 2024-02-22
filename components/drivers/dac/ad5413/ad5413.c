/**
 * @file ad5413.c
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ad5413.h"

#define SPI_CLOCK_SPEED 8000000 // 8MHz

static const char TAG[] = "adc5413";

static spi_device_handle_t s_spi;

void ad5413_init(spi_host_device_t host_id, gpio_num_t cs)
{
    esp_err_t ret = ESP_OK;

    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = SPI_CLOCK_SPEED,
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    /* Attach the device to the SPI bus */
    ret |= spi_bus_add_device(host_id, &dev_cfg, &s_spi);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device");
    }
}