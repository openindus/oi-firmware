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
#include "driver/i2c_master.h"
#include "STM32DriversComponents/M41T62.h"
#include "freertos/FreeRTOS.h"

/**
 * @brief Wrapper for i2c read operation rtc
 * @param[in] RegAddr Register to read
 * @param[in] numByteToRead Data length to read
 * @param[in] data Data read from rtc register
 * @retval true if success
 * @retval false if an error occured
 */
bool HAL_ReadReg(uint8_t RegAddr, uint32_t numByteToRead, uint8_t *data);

/**
 * @brief Wrapper for i2c write operation rtc
 * @param[in] RegAddr Register to write
 * @param[in] numByteToWrite Data length to write
 * @param[in] data Data to write
 * @retval true if success
 * @retval false if an error occured
 */
bool HAL_WriteReg(uint8_t RegAddr, uint32_t numByteToWrite, uint8_t *data);

/**
 * @brief I2C read operation of an rtc register
 * @param[in] reg Register to read
 * @param[out] data Data read from rtc register
 * @param[in] data_len Data length to read
 * @return esp_err_t
 */
esp_err_t rtc_i2c_read(uint8_t reg, uint8_t *data, size_t data_len);

/**
 * @brief I2C write operation to an rtc register
 * @param[in] reg Register to write
 * @param[in] data Data to write
 * @param[in] data_len Data length to write
 * @return esp_err_t
 */
esp_err_t rtc_i2c_write(uint8_t reg, uint8_t *data, size_t data_len);

/**
 * @brief Internal interrupt handler for RTC
 * @param arg Optional argument
 */
void rtc_i2c_begin(i2c_master_bus_handle_t i2c_master_handle, uint8_t rtc_i2c_address);

#ifdef __cplusplus
}
#endif