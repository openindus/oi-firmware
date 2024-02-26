/**
 * @file ad5413.c
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ad5413.h"

static const char TAG[] = "adc5413";

ad5413_instance_t* ad5413_hal_init(spi_host_device_t spiHost, gpio_num_t cs, uint8_t ad0, uint8_t ad1)
{
    esp_err_t err = ESP_OK;

    ad5413_instance_t* inst = malloc(sizeof(ad5413_instance_t));
    if (inst == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    spi_device_interface_config_t devCfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = APB_CLK_FREQ / 80, //SPI_MASTER_FREQ_8M,
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    /* Attach the device to the SPI bus */
    err |= spi_bus_add_device(spiHost, &devCfg, &inst->handler);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device");
        goto error;
    }

    inst->ad0 = ad0;
    inst->ad1 = ad1;
    return inst;

error:
    free(inst);
    return NULL;
}

void ad5413_hal_writeRegister(ad5413_instance_t* inst, uint8_t regAddr, uint16_t data)
{
    if (inst == NULL) {
        ESP_LOGE(TAG, "NULL instance");
        return;
    }

    uint8_t buffer[3];
    buffer[0] = (uint8_t)((~inst->ad1 << 7) | 
                          (inst->ad1 << 6) | 
                          (inst->ad0 << 5) | 
                          (regAddr & 0b00011111));
    buffer[1] = (uint8_t)((data & 0xFF00) >> 8);
    buffer[2] = (uint8_t)(data & 0x00FF);

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = 24,
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = &buffer,
        .rx_buffer = NULL
    };

    esp_err_t err = spi_device_polling_transmit(inst->handler, &trans);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register");
    }
}
