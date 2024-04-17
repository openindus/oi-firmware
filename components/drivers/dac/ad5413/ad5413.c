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
 * @brief Compute CRC8 checksum
 * @param data The data buffer
 * @param data_size The size of the data buffer
 * @return CRC8 checksum
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
 * @brief Initialize the bus SPI
 * 
 */
static void ad5413_spi_init(spi_host_device_t host_id, int clk_freq, int cs, 
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
        .clock_speed_hz = clk_freq,
        .input_delay_ns = 0,
        .spics_io_num = cs,
        .flags = 0,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL
    };

    esp_err_t err = spi_bus_add_device(host_id, &spi_conf, handle);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "Failed to add device");
}

/**
 * @brief Write to the device register
 * 
 * @param dev Device instance
 * @param reg_addr Register address
 * @param reg_data Register data
 */
static int ad5413_spi_write_reg(ad5413_device_t* dev, uint8_t reg_addr, uint16_t reg_data)
{
    if (dev == NULL) {
        goto error;
    }

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
    if (err != ESP_OK) {
        goto error;
    }

    return 0;

error:
    ESP_LOGE(TAG, "Failed to write register");
    return -1;
}

/**
 * @brief Read register from device (Two stage readback)
 * @param dev Device instance
 * @param reg_addr Register address
 * @param reg_data Register data
 * @return 0 on success, -1 on error
 */
static int ad5413_spi_read_reg(ad5413_device_t* dev, uint8_t reg_addr, uint16_t *reg_data)
{
    if (dev == NULL) {
        goto error;
    }

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

	if (dev->crc_en) {
        tx_buf[3] = ad5413_compute_crc8(tx_buf, 3);
    } else {
        tx_buf[3] = 0x00;
    }

    err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

    /* Second stage */

    tx_buf[0] = (uint8_t)((dev->slip_bit << 7) | 
                          (dev->address << 5) | 
                          (AD5413_REG_NOP & 0x1F));
	tx_buf[1] = 0x00;
	tx_buf[2] = 0x00;

	if (dev->crc_en) {
        tx_buf[3] = ad5413_compute_crc8(tx_buf, 3);
    } else {
        tx_buf[3] = 0x00;
    }

    trans.rxlength = 32;
    err = spi_device_polling_transmit(dev->spi_handler, &trans);
    if (err != ESP_OK) {
        goto error;
    }

	if ((dev->crc_en) && (ad5413_compute_crc8(rx_buf, 3) != rx_buf[3])) {
        goto error;
    }

	*reg_data = (rx_buf[1] << 8) | rx_buf[2];

	return 0;

error:
    ESP_LOGE(TAG, "Failed to write register");
    return -1;
}

/**
 * @brief Initialize the device
 * 
 * @param dev Device instance
 * @param conf Device config
 * @return int 
 */
int ad5413_init(ad5413_device_t** dev, ad5413_config_t* conf)
{
    ad5413_device_t* device = malloc(sizeof(ad5413_device_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    ad5413_spi_init(conf->host_id, conf->sclk_freq, conf->sync_pin, &device->spi_handler);

    /* Configure LDAC pin */
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << conf->ldac_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,  //An internal pull-down resistor is present into the DAC
        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&gpio_conf));

    device->slip_bit = ~conf->ad1;
    device->address = ((conf->ad1 << 1) | (conf->ad0));
    device->crc_en = true;

    *dev = device;
    return 0;

error:
    free(dev);
    return -1;
}

/**
 * @brief Write DAC data to the input register
 * 
 * @param dev Device instance
 * @param data 14-bit DAC input data
 * Accepted values: 0x00 to 0xFFFF
 * @return 0 on success, -1 on error
 */
int ad5413_dac_input_write(ad5413_device_t* dev, uint16_t data)
{
    uint16_t reg_data = ((data << AD5413_DAC_INPUT_POS) & AD5413_DAC_INPUT_MSK);

	int ret = ad5413_spi_write_reg(dev, AD5413_REG_DAC_INPUT, reg_data);
	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Read DAC data from the output register
 * 
 * @param dev Device instance
 * @param data 14-bit DAC output data
 * @return 0 on success, -1 on error
 */
int ad5413_dac_output_read(ad5413_device_t* dev, uint16_t* data)
{
    uint16_t reg_data = 0;
    int ret = ad5413_spi_read_reg(dev, AD5413_REG_DAC_OUTPUT, &reg_data);
    if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

    *data = ((reg_data & AD5413_DAC_OUTPUT_MSK) >> AD5413_DAC_OUTPUT_POS);

    return 0;
}

/**
 * @brief DAC configuration
 * 
 * @param dev Device instance
 * @param pos Bit position
 * @param msk Bit mask
 * @param data Register data
 * @return 0 on success, -1 on error
 */
int ad5413_set_dac_config(ad5413_device_t* dev, uint8_t pos, uint8_t msk, uint16_t data)
{
	int ret = 0;
    uint16_t reg_data;

    ret |= ad5413_spi_read_reg(dev, AD5413_REG_DAC_CONFIG, &reg_data);

	reg_data &= ~(msk << pos);
	reg_data |= ((data & msk) << pos);

	ret |= ad5413_spi_write_reg(dev, AD5413_REG_DAC_CONFIG, reg_data);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Select Output Range
 * 
 * @param dev Device instance
 * @param range output range
 * @return 0 on success, -1 on error
 */
int ad5758_set_output_range(ad5413_device_t* dev, ad5413_output_range_t range)
{
	int ret = 0;
    
    ret |= ad5413_set_dac_config(dev, AD5413_DAC_CONFIG_RANGE_POS, 
        AD5413_DAC_CONFIG_RANGE_MSK, range);

    ret |= ad5413_wait_for_refresh_cycle(dev);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Enable/disable Enable Internal Buffers
 * 
 * @param dev Device instance
 * @param enable Enable or disable
 * Accepted values: 0: disable
 * 		            1: enable
 * @return 0 on success, -1 on error
 */
int ad5413_internal_buffers_en(ad5413_device_t* dev, uint8_t enable)
{
	int ret = 0;
    
    ret |= ad5413_set_dac_config(dev, AD5413_DAC_CONFIG_INT_EN_POS, 
        AD5413_DAC_CONFIG_INT_EN_MSK, enable);

    ret |= ad5413_wait_for_refresh_cycle(dev);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Enable/disable VIOUT
 * 
 * @param dev Device instance
 * @param enable - enable or disable VIOUT output
 * Accepted values: 0: disable
 * 		            1: enable
 * @return 0 on success, -1 on error
 */
int ad5413_dac_out_en(ad5413_device_t* dev, uint8_t enable)
{
	int ret = ad5413_set_dac_config(dev, AD5413_DAC_CONFIG_OUT_EN_POS, 
        AD5413_DAC_CONFIG_OUT_EN_MSK, enable);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Perform a software LDAC command
 * 
 * @param dev Device instance
 * @return 0 on success, -1 on error
 */
int ad5413_soft_ldac_cmd(ad5413_device_t* dev)
{
	int ret = ad5413_spi_write_reg(dev, AD5413_REG_SW_LDAC, AD5413_SW_LDAC_COMMAND);
	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Perform a software reset
 * 
 * @param dev Device instance
 * @return 0 on success, -1 on error
 */
int ad5413_soft_reset(ad5413_device_t* dev)
{
	int ret = 0;

	ret |= ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_RESET_1);

	ret |= ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_RESET_2);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	/* Wait 100 us */
	ad5413_delay_us(100);

	return 0;
}

/**
 * @brief Perform a calibration memory refresh to the shadow registers
 * 
 * @param dev Device instance
 * @return 0 on success, -1 on error
 */
int ad5413_calib_mem_refresh(ad5413_device_t* dev)
{
	int ret = ad5413_spi_write_reg(dev, AD5413_REG_KEY,
				   AD5413_KEY_CODE_CALIB_MEM_REFRESH);
	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return ad5413_wait_for_refresh_cycle(dev);
}

/**
 * @brief Wait until the CAL_MEM_UNREFRESHED bit in the DIGITAL_DIAG_RESULTS
 * register returns to 0.
 * 
 * @param dev Device instance
 * @return 0 on success
 */
int ad5413_wait_for_refresh_cycle(ad5413_device_t* dev)
{
	uint16_t reg_data;
	do {
		ad5413_spi_read_reg(dev, AD5413_REG_DIGITAL_DIAG_RESULTS, &reg_data);
	} while (reg_data & AD5413_DIGITAL_DIAG_RESULTS_CAL_MEM_UNREFRESHED_MSK);

	return 0;
}

/**
 * @brief Clear the error flags for the on-chip digital diagnostic features
 * 
 * @param dev Device instance
 * @param flag Which flag to clear
 * @return 0 on success, -1 on error
 */
int ad5413_clear_dig_diag_flag(ad5413_device_t* dev, ad5413_dig_diag_flags_t flag)
{
	int ret = 0;
    uint16_t reg_data;

    ret |= ad5413_spi_read_reg(dev, AD5413_REG_DIGITAL_DIAG_RESULTS, &reg_data);

	reg_data &= ~(1UL << flag);
	reg_data |= (1UL << flag);

	ad5413_spi_write_reg(dev, AD5413_REG_DIGITAL_DIAG_RESULTS, reg_data);

	if (ret != 0) {
		ESP_LOGE(TAG, "%s: Failed.", __func__);
		return -1;
	}

	return 0;
}