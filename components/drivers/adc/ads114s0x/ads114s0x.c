/**
 * @file ads114s0x.c
 * @brief ADS114S0x ADC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ads114s0x.h"

static const char TAG[] = "ads114s0x";

/**
 * @brief Initialize the bus SPI
 * 
 * @param host_id SPI host device
 * @param sclk_freq clock frequency
 * @param cs chip select
 * @param handle SPI handler
 * @return int 0=success, -1=error
 */
static int ads114s0x_spi_init(spi_host_device_t host_id, int sclk_freq, int cs, 
    spi_device_handle_t *handle)
{
    spi_device_interface_config_t spi_conf = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 1,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = sclk_freq,
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    esp_err_t err = spi_bus_add_device(host_id, &spi_conf, handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device");
        return -1;
    }

    return 0;
}

/**
 * @brief Initialize the device
 * 
 * @param dev Device instance
 * @param conf Device config
 * @return int 0=success, -1=error
 */
int ads114s0x_init(ads114s0x_device_t** dev, ads114s0x_config_t* conf)
{
    ads114s0x_device_t* device = malloc(sizeof(ads114s0x_device_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    /** @todo: init SPI and GPIOs */
    int err = 0;
    err |= ads114s0x_spi_init(conf->host_id, conf->sclk_freq, conf->cs, &device->spi_handler);
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to initialize the SPI");
        goto error;
    }

    *dev = device;
    return 0;

error:
    free(device);
    return -1;
}