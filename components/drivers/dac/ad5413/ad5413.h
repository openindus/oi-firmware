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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Register address map
 * 
 */
typedef enum {
    AD5413_REG_ADDR_NOP                         = 0x00,
    AD5413_REG_ADDR_DAC_INPUT                   = 0x01,
    AD5413_REG_ADDR_DAC_OUTPUT                  = 0x02,
    AD5413_REG_ADDR_CLEAR_CODE                  = 0x03,
    AD5413_REG_ADDR_USER_GAIN                   = 0x04,
    AD5413_REG_ADDR_USER_OFFSET                 = 0x05,
    AD5413_REG_ADDR_DAC_CONFIG                  = 0x06,
    AD5413_REG_ADDR_SW_LDAC                     = 0x07,
    AD5413_REG_ADDR_KEY                         = 0x08,
    AD5413_REG_ADDR_GP_CONFIG1                  = 0x09,
    AD5413_REG_ADDR_GP_CONFIG2                  = 0x0A,
    AD5413_REG_ADDR_DIGITAL_DIAG_CONFIG         = 0x10,
    AD5413_REG_ADDR_FAULT_PIN_CONFIG            = 0x12,
    AD5413_REG_ADDR_TWO_STAGE_READBACK_SELECT   = 0x13,
    AD5413_REG_ADDR_DIGITAL_DIAG_RESULTS        = 0x14,
    AD5413_REG_ADDR_ANALOG_DIAG_RESULTS         = 0x15,
    AD5413_REG_ADDR_STATUS                      = 0x16,
    AD5413_REG_ADDR_CHIP_ID                     = 0x17,
    AD5413_REG_ADDR_FREQ_MONITOR                = 0x18,
    AD5413_REG_ADDR_DEVICE_ID_3                 = 0x1C
} ad5413_regAddr_t;

/**
 * @brief Device instance
 * 
 */
typedef struct {
    spi_device_handle_t handler;
    uint8_t ad0 : 1;
    uint8_t ad1 : 1;
} ad5413_instance_t;

/**
 * @brief Initialize the device
 * 
 * @param spiHost SPI host
 * @param cs Chip select (SYNC)
 * @param ad0 Address decode 0
 * @param ad0 Address decode 1
 * @return ad5413_instance_t* 
 */
ad5413_instance_t* ad5413_hal_init(spi_host_device_t spiHost, gpio_num_t cs, uint8_t ad0, uint8_t ad1);

/**
 * @brief Write to a register
 * 
 * @param data 
 */

/**
 * @brief Write to a register
 * 
 * @param inst Device instance
 * @param regAddr Register address
 * @param data data to write
 */
void ad5413_hal_writeRegister(ad5413_instance_t* inst, uint8_t regAddr, uint16_t data);

#ifdef __cplusplus
}
#endif