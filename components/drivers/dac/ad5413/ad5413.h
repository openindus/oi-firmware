/**
 * @file ad5413.h
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* AD5413 registers */
#define AD5413_REG_NOP						    0x00
#define AD5413_REG_DAC_INPUT					0x01
#define AD5413_REG_DAC_OUTPUT					0x02
#define AD5413_REG_CLEAR_CODE					0x03
#define AD5413_REG_USER_GAIN					0x04
#define AD5413_REG_USER_OFFSET					0x05
#define AD5413_REG_DAC_CONFIG					0x06
#define AD5413_REG_SW_LDAC					    0x07
#define AD5413_REG_KEY						    0x08
#define AD5413_REG_GP_CONFIG1					0x09
#define AD5413_REG_GP_CONFIG2					0x0A
#define AD5413_REG_DIGITAL_DIAG_CONFIG		    0x10
#define AD5413_REG_FAULT_PIN_CONFIG				0x12
#define AD5413_REG_TWO_STAGE_READBACK_SELECT    0x13
#define AD5413_REG_DIGITAL_DIAG_RESULTS		    0x14
#define AD5413_REG_ANALOG_DIAG_RESULTS		    0x15
#define AD5413_REG_STATUS					    0x16
#define AD5413_REG_CHIP_ID					    0x17
#define AD5413_REG_FREQ_MONITOR					0x18
#define AD5413_REG_DEVICE_ID_3					0x1C

/* AD5413_REG_DAC_INPUT */
#define AD5413_DAC_INPUT_SET(x)			        ((x & 0x3FFF) << 2)

/* AD5413_REG_DAC_OUTPUT */
#define AD5413_DAC_OUTPUT_GET(x)			    ((x & 0xFFFC) >> 2)

/* AD5413_REG_DAC_CONFIG */
#define AD5413_DAC_CONFIG_RANGE_MSK			    (0xF << 0)
#define AD5413_DAC_CONFIG_RANGE_SET(x)			((x & 0xF) << 0)
#define AD5413_DAC_CONFIG_OVRNG_EN_MSK			(0x1 << 4)
#define AD5413_DAC_CONFIG_OVRNG_EN_SET(x)		((x & 0x1) << 4)
#define AD5413_DAC_CONFIG_INT_EN_MSK			(0x1 << 5)
#define AD5413_DAC_CONFIG_INT_EN_SET(x)		    ((x & 0x1) << 5)
#define AD5413_DAC_CONFIG_OUT_EN_MSK			(0x1 << 6)
#define AD5413_DAC_CONFIG_OUT_EN_SET(x)		    ((x & 0x1) << 6)
#define AD5413_DAC_CONFIG_RSET_EXT_EN_MSK		(0x1 << 7)
#define AD5413_DAC_CONFIG_RSET_EXT_EN_SET(x)	((x & 0x1) << 7)
#define AD5413_DAC_CONFIG_SR_EN_MSK			    (0x1 << 8)
#define AD5413_DAC_CONFIG_SR_EN_SET(x)			((x & 0x1) << 8)
#define AD5413_DAC_CONFIG_SR_CLOCK_MSK			(0xF << 9)
#define AD5413_DAC_CONFIG_SR_CLOCK_SET(x)		((x & 0xF) << 9)
#define AD5413_DAC_CONFIG_SR_STEP_MSK			(0x7 << 13)
#define AD5413_DAC_CONFIG_SR_STEP_SET(x)		((x & 0x7) << 13)

/* AD5413_REG_SW_LDAC */
#define AD5413_SW_LDAC_COMMAND				    0x1DAC

/* AD5413_REG_KEY */
#define AD5413_KEY_CODE_RESET_1				    0x15FA
#define AD5413_KEY_CODE_RESET_2				    0xAF51
#define AD5413_KEY_CODE_CALIB_MEM_REFRESH		0xFCBA

/* AD5413_REG_DIGITAL_DIAG_RESULTS */
#define AD5413_DIG_DIAG_RESULTS_SPI_CRC_ERR_MSK			        (0x1 << 0)
#define AD5413_DIG_DIAG_RESULTS_SLIPBIT_ERR_ERR_MSK			    (0x1 << 1)
#define AD5413_DIG_DIAG_RESULTS_SCLK_COUNT_ERR_ERR_MSK		    (0x1 << 2)
#define AD5413_DIG_DIAG_RESULTS_INVALID_SPI_ACC_ERR_ERR_MSK     (0x1 << 4)
#define AD5413_DIG_DIAG_RESULTS_CAL_MEM_CRC_ERR_ERR_MSK		    (0x1 << 5)
#define AD5413_DIG_DIAG_RESULTS_INV_DAC_CHECK_ERR_ERR_MSK       (0x1 << 6)
#define AD5413_DIG_DIAG_RESULTS_DAC_LATCH_MON_ERR_ERR_MSK       (0x1 << 8)
#define AD5413_DIG_DIAG_RESULTS_RES_OCCURRED_MSK		        (0x1 << 13)
#define AD5413_DIG_DIAG_RESULTS_SLEW_BUSY_MSK			        (0x1 << 14)
#define AD5413_DIG_DIAG_RESULTS_CAL_MEM_UNREFRESHED_MSK		    (0x1 << 15)

typedef enum {
	DIG_DIAG_SPI_CRC_ERR = 0,
	DIG_DIAG_SLIPBIT_ERR = 1,
	DIG_DIAG_SCLK_COUNT_ERR = 2,
	DIG_DIAG_INVALID_SPI_ACCESS_ERR = 4,
	DIG_DIAG_CAL_MEM_CRC_ERR = 5,
	DIG_DIAG_INVERSE_DAC_CHECK_ERR = 6,
	DIG_DIAG_DAC_LATCH_MON_ERR = 8,
	DIG_DIAG_RESET_OCCURRED = 13,
} ad5413_dig_diag_flags_t;

typedef enum {
	OUTPUT_RANGE_M10V5_10V5 = 0b0011,
	OUTPUT_RANGE_0mA_20mA = 0b1001
} ad5413_output_range_t;

typedef struct {
    spi_host_device_t host_id;  // SPI host Id
    int sclk_freq;              // Clock frequency
    gpio_num_t sync;            // SYNC pin (Chip select)
    uint8_t ad0 : 1;            // Address decode 0
    uint8_t ad1 : 1;            // Address decode 1
} ad5413_config_t;

typedef struct {
    spi_device_handle_t spi_handler;
    uint8_t slip_bit : 1;
    uint8_t address : 2;
    uint8_t crc_en;
} ad5413_device_t;

ad5413_device_t* ad5413_init(ad5413_config_t conf);

int ad5413_dac_input_write(ad5413_device_t* dev, uint16_t data);
int ad5413_dac_output_read(ad5413_device_t* dev, uint16_t* data);
int ad5413_soft_reset(ad5413_device_t* dev);
int ad5413_calib_mem_refresh(ad5413_device_t* dev);
int ad5413_wait_for_refresh_cycle(ad5413_device_t* dev);
int ad5413_clear_dig_diag_flag(ad5413_device_t* dev, ad5413_dig_diag_flags_t flag);
int ad5413_dac_output_en(ad5413_device_t* dev, uint8_t enable);
int ad5413_soft_ldac_cmd(ad5413_device_t* dev);
int ad5413_internal_buffers_en(ad5413_device_t* dev, uint8_t enable);
int ad5758_set_output_range(ad5413_device_t* dev, ad5413_output_range_t range);

#ifdef __cplusplus
}
#endif