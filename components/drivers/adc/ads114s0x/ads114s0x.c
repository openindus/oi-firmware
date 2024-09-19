/**
 * @file ads114s0x.c
 * @brief ADS114S0x ADC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ads114s0x.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
    ((byte) & 0x80 ? '1' : '0'), \
    ((byte) & 0x40 ? '1' : '0'), \
    ((byte) & 0x20 ? '1' : '0'), \
    ((byte) & 0x10 ? '1' : '0'), \
    ((byte) & 0x08 ? '1' : '0'), \
    ((byte) & 0x04 ? '1' : '0'), \
    ((byte) & 0x02 ? '1' : '0'), \
    ((byte) & 0x01 ? '1' : '0') 

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
    esp_err_t err = ESP_OK;
    ads114s0x_device_t* device = malloc(sizeof(ads114s0x_device_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    /* GPIO init */
    gpio_config_t output_conf = {
        .pin_bit_mask = 
            (1ULL << conf->start_sync) | 
            (1ULL << conf->reset),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    err |= gpio_config(&output_conf);

    gpio_config_t input_conf = {
        .pin_bit_mask = (1ULL << conf->drdy),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    err |= gpio_config(&input_conf);

    /* SPI init */
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
    err |= spi_bus_add_device(conf->host_id, &spi_conf, &device->spi_handler);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the device");
        goto error;
    }

    device->config = conf;
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
int ads114s0x_read_data(ads114s0x_device_t* dev, int* data)
{
    if ((dev == NULL) || data == NULL) {
        goto error;
    }

    /**
     * @todo STATUS and CRC bytes are not implemented
     * 
     */

    uint8_t buffer[2] = {0};

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = (uint16_t)(ADS114S0X_CMD_RDATA << 8),
        .addr = 0,
        .length = 16,
        .rxlength = 16,
        .user = NULL,
        .tx_buffer = NULL,
        .rx_buffer = buffer
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }
    
    *data = ((buffer[0] << 8) | buffer[1]);
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

/**
 * @brief Perform start/sync
 * 
 * @param dev Device instance
 * @return int 0=success, -1=error
 */
int ads114s0x_start_sync(ads114s0x_device_t* dev)
{
    if (dev == NULL) {
        return -1;
    }

    esp_err_t err = ESP_OK;
    err |= gpio_set_level(dev->config->start_sync, 1);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    err |= gpio_set_level(dev->config->start_sync, 0);
    if (err != ESP_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief Perform hardware reset
 * 
 * @param dev Device instance
 * @return int 0=success, -1=error
 */
int ads114s0x_hard_reset(ads114s0x_device_t* dev)
{
    if (dev == NULL) {
        return -1;
    }

    esp_err_t err = ESP_OK;
    err |= gpio_set_level(dev->config->reset, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    err |= gpio_set_level(dev->config->reset, 1);
    if (err != ESP_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief Enable device
 * 
 * @param dev Device instance
 * @return int 0=success, -1=error
 */
int ads114s0x_enable(ads114s0x_device_t* dev)
{
    if (dev == NULL) {
        return -1;
    }

    esp_err_t err = gpio_set_level(dev->config->reset, 1);
    if (err != ESP_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief Disable device
 * 
 * @param dev Device instance
 * @return int 0=success, -1=error
 */
int ads114s0x_disable(ads114s0x_device_t* dev)
{
    if (dev == NULL) {
        return -1;
    }

    esp_err_t err = gpio_set_level(dev->config->reset, 0);
    if (err != ESP_OK) {
        return -1;
    } else {
        return 0;
    }
}

/**
 * @brief Add ISR handler for DRDY
 * 
 * @param dev Device instance
 * @param isr_handler ISR handler
 * @param args Arguments
 * @return int 0=success, -1=error
 */
int ads114s0x_add_data_ready_isr_handler(ads114s0x_device_t* dev, gpio_isr_t isr_handler, void* args)
{
    if (dev == NULL) {
        return -1;
    }

    esp_err_t err = gpio_isr_handler_add(dev->config->drdy, isr_handler, args);
    if (err != ESP_OK) {
        return -1;
    } else {
        return 0;
    }
}

void ads114s0x_print_register_map(ads114s0x_device_t* dev)
{
    ads114s0x_register_map_t map;
    ads114s0x_read_register(dev, 0x00, (uint8_t*)&map, sizeof(ads114s0x_register_map_t));
    uint8_t byte[18];
    memcpy(byte, (uint8_t*)&map, 18);
    for (int i=0; i<18; i++) {
        printf("[0x%02x]:\t%c%c%c%c%c%c%c%c\n", i, BYTE_TO_BINARY(byte[i]));
    }
}