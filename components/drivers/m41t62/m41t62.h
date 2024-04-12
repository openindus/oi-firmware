/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file m41t62.h
 * @brief Functions to the rtc m41t62
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "STM32DriversComponents/M41T62.h"

typedef void (*rtc_isr_t)(void *);

extern rtc_isr_t rtc_user_handler;

/**
 * @brief Wrapper for i2c read operation rtc
 * @param[in] i2cAddr RTC address on I2C bus
 * @param[in] RegAddr Register to read
 * @param[in] numByteToRead Data length to read
 * @param[in] data Data read from rtc register
 * @retval true if success
 * @retval false if an error occured
 */
bool HAL_ReadReg(uint8_t i2cAddr ,uint8_t RegAddr, uint32_t numByteToRead, uint8_t *data);

/**
 * @brief Wrapper for i2c write operation rtc
 * @param[in] i2cAddr RTC address on I2C bus
 * @param[in] RegAddr Register to write
 * @param[in] numByteToWrite Data length to write
 * @param[in] data Data to write
 * @retval true if success
 * @retval false if an error occured
 */
bool HAL_WriteReg(uint8_t i2cAddr, uint8_t RegAddr, uint32_t numByteToWrite, uint8_t *data);

/**
 * @brief I2C read operation of an rtc register 
 * @param[in] i2c_port I2C port number
 * @param[in] address RTC address on I2C bus
 * @param[in] reg Register to read
 * @param[out] data Data read from rtc register
 * @param[in] data_len Data length to read
 * @return esp_err_t
 */
esp_err_t rtc_i2c_read(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data, size_t data_len);

/**
 * @brief I2C write operation to an rtc register 
 * @param[in] i2c_port I2C port number
 * @param[in] address RTC address on I2C bus
 * @param[in] reg Register to write
 * @param[in] data Data to write
 * @param[in] data_len Data length to write
 * @return esp_err_t
 */
esp_err_t rtc_i2c_write(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data, size_t data_len);

/**
 * @brief Internal interrupt handler for RTC
 * @param arg Optional argument
 */
void rtc_isr_handler(void* arg);

/**
 * @brief Internal interrupt handler for RTC
 * @param arg Optional argument
 */
void rtc_i2c_set_port(i2c_port_t i2c_port);

#ifdef __cplusplus
}
#endif