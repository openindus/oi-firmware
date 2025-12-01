/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file m41t62.c
 * @brief Functions to the rtc m41t62
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "m41t62/m41t62.h"

static const char RTC_TAG[] = "RTC";
i2c_port_t rtc_i2c_port = -1;

#define RTC_CHECK(a, str, goto_tag, ...)                                                    \
    do                                                                                      \
    {                                                                                       \
        if (a)                                                                              \
        {                                                                                   \
            ESP_LOGE(RTC_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__);       \
            goto goto_tag;                                                                  \
        }                                                                                   \
    } while (0)

#define ACK_CHECK_EN                    (0x01)  /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                   (0x00)  /*!< I2C master will not check ack from slave */
#define ACK_VAL                         (0x00)  /*!< I2C ack value */
#define NACK_VAL                        (0x01)  /*!< I2C nack value */

///////////////////////////////ST I2C INTERFACE/////////////////////////////////////////////////////////////////////////

bool HAL_ReadReg(uint8_t i2cAddr ,uint8_t regAddr, uint32_t numByteToRead, uint8_t *data)
{
    esp_err_t ret = rtc_i2c_read(rtc_i2c_port, i2cAddr, regAddr, data, numByteToRead);
    if(ret == ESP_OK)
    {
        return false;
    }

    return true;
}

bool HAL_WriteReg(uint8_t i2cAddr, uint8_t regAddr, uint32_t numByteToWrite, uint8_t *data)
{
    esp_err_t ret = rtc_i2c_write(rtc_i2c_port, i2cAddr, regAddr, data, numByteToWrite);
    if(ret == ESP_OK)
    {
        return false;
    }

    return true;
} 

///////////////////////////////ESP I2C FUNCTIONS/////////////////////////////////////////////////////////////////////////
esp_err_t rtc_i2c_write(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data, size_t data_len)
{
    ESP_LOGV(RTC_TAG, "WRITE: i2c_port:%#04x; device_address:%#04x; register:%#04x;", i2c_port, address, reg);

    esp_err_t ret = ESP_OK;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be written
    i2c_master_write_byte(cmd, ( address | I2C_MASTER_WRITE), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    // write the data
    i2c_master_write(cmd, data, data_len, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t rtc_i2c_read(i2c_port_t i2c_port, uint8_t address, uint8_t reg, uint8_t *data, size_t data_len)
{
    ESP_LOGV(RTC_TAG, "READ: i2c_port:%#04x; device_address:%#04x; register:%#04x", i2c_port, address, reg);
    esp_err_t ret = ESP_OK;
    if (data_len == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    // first, send device address (indicating write) & register to be read
    i2c_master_write_byte(cmd, ( address | I2C_MASTER_WRITE), ACK_CHECK_EN);
    // send register we want
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    // Send repeated start
    i2c_master_start(cmd);
    // now send device address (indicating read) & read data
    i2c_master_write_byte(cmd, ( address | I2C_MASTER_READ), ACK_CHECK_EN);
    if (data_len > 1) {
        i2c_master_read(cmd, data, data_len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data + data_len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void rtc_i2c_set_port(i2c_port_t i2c_port)
{
    rtc_i2c_port = i2c_port;
}