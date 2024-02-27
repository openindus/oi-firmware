/**
 * @file ad5413.c
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ad5413.h"

static const char TAG[] = "adc5413";

/**
 * @brief Compute CRC8 checksum.
 * @param data The data buffer.
 * @param data_size The size of the data buffer.
 * @return CRC8 checksum.
 */
static uint8_t ad5758_compute_crc8(uint8_t *data, uint8_t data_size)
{
	uint8_t i;
	uint8_t crc = 0;

	while (data_size) {
		for (i = 0x80; i != 0; i >>= 1) {
			if (((crc & 0x80) != 0) != ((*data & i) != 0)) {
				crc <<= 1;
				crc ^= 0x07; // x^8 + x^2 + x^1 + 1
			} else {
				crc <<= 1;
			}
		}
		data++;
		data_size--;
	}

	return crc;
}

/**
 * @brief Initialize the bus SPI.
 * 
 */
void ad5413_spi_init(spi_host_device_t host_id, int clk_freq, int cs, spi_device_handle_t *handle)
{
    spi_device_interface_config_t spi_conf = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = clk_freq,
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    /* Attach the device to the SPI bus */
    esp_err_t err = spi_bus_add_device(host_id, &spi_conf, handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add device");
    }
}

/**
 * @brief Write to the device register.
 * 
 * @param dev Device instance.
 * @param reg_addr Register address.
 * @param reg_data Register data.
 */
void ad5413_spi_write_reg(ad5413_device_t* dev, uint8_t reg_addr, uint16_t reg_data)
{
    if (dev == NULL) {
        ESP_LOGE(TAG, "Ptr NULL");
        return;
    }

    uint8_t buf[4];
    buf[0] = (uint8_t)((dev->slip_bit << 7) | 
                       (dev->address << 5) | 
                       (reg_addr & 0x1F));
    buf[1] = (uint8_t)(reg_data >> 8);
    buf[2] = (uint8_t)(reg_data & 0xFF);
    buf[3] = ad5758_compute_crc8(buf, 3);

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = 32,
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = &buf,
        .rx_buffer = NULL
    };

    esp_err_t err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write register");
    }
}

/**
 * @brief Initialize the device.
 * 
 * @param conf Device config.
 * @return ad5413_device_t* device.
 */
ad5413_device_t* ad5413_init(ad5413_config_t conf)
{
    ad5413_device_t* dev = malloc(sizeof(ad5413_device_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    ad5413_spi_init(conf.host_id, conf.sclk_freq, conf.sync, &dev->spi_handler);

    dev->slip_bit = ~conf.ad1;
    dev->address = ((conf.ad1 << 1) | (conf.ad0));

    return dev;

error:
    free(dev);
    return NULL;
}