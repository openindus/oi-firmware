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

    /** @todo: init GPIOs */

    /* SPI init*/
    spi_device_interface_config_t spi_conf = {
        .command_bits = 16,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 1,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = conf->sclk_freq,
        .input_delay_ns = 0,
        .spics_io_num = conf->cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    esp_err_t err = spi_bus_add_device(conf->host_id, &spi_conf, &device->spi_handler);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize th SPI device");
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
static int ads114s0x_command(ads114s0x_device_t* dev, uint8_t cmd)
{
    if (dev == NULL) {
        goto error;
    }

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = (uint16_t)(cmd << 8),
        .addr = 0,
        .length = 8,
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = NULL,
        .rx_buffer = NULL
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    return 0;

error:
    ESP_LOGE(TAG, "Failed to perform control or calibration command");
    return -1;
}

int ads114s0x_wakeup(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_WAKEUP);
}

int ads114s0x_powerdown(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_POWERDOWN);
}

int ads114s0x_reset(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_RESET);
}

int ads114s0x_start(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_START);
}

int ads114s0x_stop(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_STOP);
}

int ads114s0x_system_offset_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_SYOCAL);
}

int ads114s0x_system_gain_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_SYGCAL);
}

int ads114s0x_self_offset_calib(ads114s0x_device_t* dev)
{
    return ads114s0x_command(dev, ADS114S0X_CMD_SFOCAL);
}

/**
 * @brief Read data
 * 
 * @param dev Device instance
 * @param data ADC data Bytes
 * @return int 0=success, -1=error
 */
int ads114s0x_read_data(ads114s0x_device_t* dev, uint16_t* data)
{
    if (dev == NULL) {
        goto error;
    }

    /**
     * @todo STATUS and CRC bytes are not implemented
     * 
     */

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = (uint16_t)(ADS114S0X_CMD_RDATA),
        .addr = 0,
        .length = 16,
        .rxlength = 16,
        .user = NULL,
        .tx_buffer = NULL,
        .rx_buffer = &data
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    return 0;

error:
    ESP_LOGE(TAG, "Failed to read register");
    return -1;
}

/**
 * @brief Read register
 * 
 * @param dev Device instance
 * @param reg_addr Register address
 * @param reg_data Register data
 * @param reg_size Register size
 * @return int 0=success, -1=error
 */
int ads114s0x_read_register(ads114s0x_device_t* dev, uint8_t reg_addr, uint8_t* reg_data, size_t reg_size)
{
    size_t len = reg_size * sizeof(uint8_t);
    uint8_t* buf = (uint8_t*)malloc(reg_size * sizeof(uint8_t));

    if ((dev == NULL) || (buf == NULL)) {
        goto error;
    }

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = (uint16_t)(((ADS114S0X_CMD_RREG | (reg_addr & 0x1F)) << 8) | ((reg_size - 1) & 0xFF)),
        .addr = 0,
        .length = (reg_size * 8),
        .rxlength = (reg_size * 8),
        .user = NULL,
        .tx_buffer = NULL,
        .rx_buffer = buf
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    memcpy(reg_data, buf, len); 
    free(buf);   
    return 0;

error:
    ESP_LOGE(TAG, "Failed to read register");
    free(buf);
    return -1;
}

/**
 * @brief Write register
 * 
 * @param dev Device instance
 * @param reg_addr Register address
 * @param reg_data Register data
 * @param reg_size Register size
 * @return int 0=success, -1=error
 */
int ads114s0x_write_register(ads114s0x_device_t* dev, uint8_t reg_addr, uint8_t* reg_data, size_t reg_size)
{
    size_t len = reg_size * sizeof(uint8_t);
    uint8_t* buf = (uint8_t*)malloc(reg_size * sizeof(uint8_t));
    memcpy(buf, reg_data, len); 

    if ((dev == NULL) || (buf == NULL)) {
        goto error;
    }

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = (uint16_t)(((ADS114S0X_CMD_WREG | (reg_addr & 0x1F)) << 8) | ((reg_size - 1) & 0xFF)),
        .addr = 0,
        .length = (reg_size * 8),
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = buf,
        .rx_buffer = NULL
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    free(buf);   
    return 0;

error:
    ESP_LOGE(TAG, "Failed to read register");
    free(buf);
    return -1;
}
