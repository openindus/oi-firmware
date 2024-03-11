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

/**
 * @brief Perform a control or calibration command
 * 
 * @param dev Device instance
 * @param cmd Command
 * @return int 0=success, -1=error
 */
static int ads114s0x_ctrl_calib_cmd(ads114s0x_device_t* dev, uint8_t cmd)
{
    if (dev == NULL) {
        goto error;
    }

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = 8,
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = &cmd,
        .rx_buffer = NULL
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    return 0;

error:
    ESP_LOGE(TAG, "Failed to perform SPI transaction");
    return -1;
}

int ads114s0x_wakeup(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_WAKEUP);
}

int ads114s0x_powerdown(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_POWERDOWN);
}

int ads114s0x_reset(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_RESET);
}

int ads114s0x_start(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_START);
}

int ads114s0x_stop(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_STOP);
}

int ads114s0x_system_offset_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_SYOCAL);
}

int ads114s0x_system_gain_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_SYGCAL);
}

int ads114s0x_self_offset_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_ctrl_calib_cmd(dev, ADS114S0X_CMD_SFOCAL);
}