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


/* AD5413_REG_KEY */
#define AD5413_KEY_CODE_RESET_1				    0x15FA
#define AD5413_KEY_CODE_RESET_2				    0xAF51
#define AD5413_KEY_CODE_CALIB_MEM_REFRESH		0xFCBA

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
int ad5413_soft_reset(ad5413_device_t* dev);
int ad5413_calib_mem_refresh(ad5413_device_t* dev);
int ad5413_wait_for_refresh_cycle(ad5413_device_t* dev);

#ifdef __cplusplus
}
#endif