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
i2c_master_dev_handle_t _i2c_dev_handle = NULL;

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

#define I2C_MASTER_FREQ_HZ              400000
#define I2C_MASTER_TIMEOUT_MS           pdMS_TO_TICKS(1000)

///////////////////////////////ST I2C INTERFACE/////////////////////////////////////////////////////////////////////////

bool HAL_ReadReg(uint8_t regAddr, uint32_t numByteToRead, uint8_t *data)
{
    esp_err_t ret = rtc_i2c_read(regAddr, data, numByteToRead);
    if(ret == ESP_OK)
    {
        return false;
    }

    return true;
}

bool HAL_WriteReg(uint8_t regAddr, uint32_t numByteToWrite, uint8_t *data)
{
    esp_err_t ret = rtc_i2c_write(regAddr, data, numByteToWrite);
    if(ret == ESP_OK)
    {
        return false;
    }

    return true;
} 

///////////////////////////////ESP I2C FUNCTIONS/////////////////////////////////////////////////////////////////////////
esp_err_t rtc_i2c_write(uint8_t reg, uint8_t *data, size_t data_len)
{
    ESP_LOGV(RTC_TAG, "WRITE: register:%#04x;", reg);

    uint8_t *write_buf = NULL;
    write_buf = (uint8_t*) malloc(sizeof(uint8_t)*(data_len+1));
    if (!write_buf) return ESP_ERR_NO_MEM;
    write_buf[0] = reg;
    memcpy(&write_buf[1], data, data_len);
    esp_err_t ret =  i2c_master_transmit(_i2c_dev_handle, write_buf, data_len+1, I2C_MASTER_TIMEOUT_MS);
    free(write_buf);
    return ret;
}

esp_err_t rtc_i2c_read(uint8_t reg, uint8_t *data, size_t data_len)
{
    ESP_LOGV(RTC_TAG, "READ: register:%#04x", reg);
    
    if (data_len == 0) {
        return ESP_OK;
    }
    return i2c_master_transmit_receive(_i2c_dev_handle, &reg, 1, data, data_len, I2C_MASTER_TIMEOUT_MS);
}

void rtc_i2c_begin(i2c_master_bus_handle_t i2c_master_handle, uint8_t rtc_i2c_address)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = rtc_i2c_address,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    i2c_master_bus_add_device(i2c_master_handle, &dev_config, &_i2c_dev_handle);
}