/**
 * @file ad5413.c
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ad5413.h"

static const char TAG[] = "adc5413";

#define NOP() asm volatile ("nop")

static void IRAM_ATTR ad5413_delay_us(uint32_t us)
{
    uint32_t m = esp_timer_get_time();
    if (us) {
        uint32_t e = (m + us);
        if (m > e) { //overflow
            while(esp_timer_get_time() > e) {
                NOP();
            }
        }
        while (esp_timer_get_time() < e) {
            NOP();
        }
    }
}

/**
 * @brief Compute CRC8 checksum.
 * @param data The data buffer.
 * @param data_size The size of the data buffer.
 * @return CRC8 checksum.
 */
static uint8_t ad5413_compute_crc8(uint8_t *data, uint8_t data_size)
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
static void ad5413_spi_init(spi_host_device_t host_id, int clk_freq, int cs, spi_device_handle_t *handle)
{
    spi_device_interface_config_t spi_conf = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 1,
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
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to add device");
}

/**
 * @brief Write to the device register.
 * 
 * @param dev Device instance.
 * @param reg_addr Register address.
 * @param reg_data Register data.
 */
static int ad5413_spi_write_reg(ad5413_device_t* dev, uint8_t reg_addr, uint16_t reg_data)
{
    if (dev == NULL)
        goto error;

    uint8_t buf[4];
    buf[0] = (uint8_t)((dev->slip_bit << 7) | 
                       (dev->address << 5) | 
                       (reg_addr & 0x1F));
    buf[1] = (uint8_t)(reg_data >> 8);
    buf[2] = (uint8_t)(reg_data & 0xFF);
    buf[3] = ad5413_compute_crc8(buf, 3);

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
    if (err != ESP_OK)
        goto error;

    return 0;

error:
    ESP_LOGE(TAG, "Failed to write register");
    return -1;
}

/**
 * @brief Read register from device (Two stage readback).
 * @param dev Device instance.
 * @param reg_addr Register address.
 * @param reg_data Register data
 * @return 0 on success, -1 on error.
 */
static int ad5413_spi_read_reg(ad5413_device_t* dev, uint8_t reg_addr, uint16_t *reg_data)
{
    if (dev == NULL)
        goto error;

    esp_err_t err = ESP_OK;
	uint8_t tx_buf[4], rx_buf[4];

    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = 0,
        .length = 32,
        .rxlength = 0,
        .user = NULL,
        .tx_buffer = &tx_buf,
        .rx_buffer = &rx_buf
    };

    /* First stage */

    tx_buf[0] = (uint8_t)((dev->slip_bit << 7) | 
                          (dev->address << 5) | 
                          (AD5413_REG_TWO_STAGE_READBACK_SELECT & 0x1F));
	tx_buf[1] = 0x00;
	tx_buf[2] = reg_addr;

	if (dev->crc_en)
		tx_buf[3] = ad5413_compute_crc8(tx_buf, 3);
	else
		tx_buf[3] = 0x00;

    err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK)
        goto error;

    /* Second stage */

    tx_buf[0] = (uint8_t)((dev->slip_bit << 7) | 
                          (dev->address << 5) | 
                          (AD5413_REG_NOP & 0x1F));
	tx_buf[1] = 0x00;
	tx_buf[2] = 0x00;

	if (dev->crc_en)
		tx_buf[3] = ad5413_compute_crc8(tx_buf, 3);
	else
		tx_buf[3] = 0x00;

    trans.rxlength = 32;
    err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) 
            goto error;        

	if ((dev->crc_en) && 
        (ad5413_compute_crc8(rx_buf, 3) != rx_buf[3]))
		goto error;

	*reg_data = (rx_buf[1] << 8) | rx_buf[2];

	return 0;

error:
    ESP_LOGE(TAG, "Failed to write register");
    return -1;
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
    dev->crc_en = true;

    return dev;

error:
    free(dev);
    return NULL;
}

/**
 * @brief Initiate a software reset
 * @param dev Device instance.
 * @return 0 on success, -1 on error.
 */
int ad5413_soft_reset(ad5413_device_t* dev)
{
	int ret = 0;

	ret = ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_RESET_1);
	if (ret != 0)
		goto error;

	ret = ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_RESET_2);
	if (ret != 0)
		goto error;

	/* Wait 100 us */
	ad5413_delay_us(100);

	return 0;

error:
	ESP_LOGE(TAG, "%s: Failed.", __func__);
	return -1;
}

/**
 * @brief Initiate a calibration memory refresh to the shadow registers
 * @param dev Device instance.
 * @return 0 on success, -1 on error.
 */
int ad5413_calib_mem_refresh(ad5413_device_t* dev)
{
	int ret;

	ret = ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_CALIB_MEM_REFRESH);
	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

    return 0;

	/* Wait to allow time for the internal calibrations to complete */
	return ad5413_wait_for_refresh_cycle(dev);
}


/**
 * @brief Busy wait until CAL_MEM_UNREFRESHED bit in the DIGITAL_DIAG_RESULTS clears
 * @param dev Device instance.
 * @return 0 on success
 */
int ad5413_wait_for_refresh_cycle(ad5413_device_t* dev)
{
	uint16_t reg_data;
	/*
	 * Wait until the CAL_MEM_UNREFRESHED bit in the DIGITAL_DIAG_RESULTS
	 * register returns to 0.
	 */
	do {
		ad5413_spi_read_reg(dev, AD5413_REG_DIGITAL_DIAG_RESULTS, &reg_data);
	} while (reg_data & (1UL << 15));

	return 0;
}