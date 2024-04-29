/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCAN.c
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BusCAN.h"

static const char TAG[] = "BusCAN";

SemaphoreHandle_t BusCAN::_mutex;

/**
 * @brief initialization of CAN communication
 * 
 * @param txNum pin
 * @param rxNum pin
 */
void BusCAN::begin(gpio_num_t txNum, gpio_num_t rxNum)
{
    ESP_LOGV(TAG, "init");

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        txNum, 
        rxNum, 
        TWAI_MODE_NORMAL
    );
    g_config.intr_flags = 0;
    g_config.rx_queue_len = 255;
        
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    /* install TWAI driver */
    ESP_LOGI(TAG, "install twai driver");
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));

    /* Start TWAI driver */
    ESP_LOGI(TAG, "start twai driver");
    ESP_ERROR_CHECK(twai_start());

#if !defined(DEBUG_BUS)
    esp_log_level_set(TAG, ESP_LOG_WARN);
#endif

}

/**
 * @brief deinitialization of CAN communication
 */
void BusCAN::end(void)
{
    ESP_LOGV(TAG, "deinit");

    /* Stop TWAI driver */
    ESP_LOGI(TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_stop());

    /* uninstall twai driver */
    ESP_LOGI(TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_driver_uninstall());
}

/**
 * @brief Write message on the CAN bus
 * 
 * @param frame 
 * @param id identifier
 * @param length data length
 * @return error: -1, succed: 0
 */
int BusCAN::write(Frame_t* frame, uint16_t id, uint8_t length)
{
    esp_err_t err;
    twai_message_t msg = {
        .flags = 0,
        .identifier = id,
        .data_length_code = length,
        .data = {}
    };
    memcpy(msg.data, frame, length);
    xSemaphoreTake(_mutex, portMAX_DELAY);
    err = twai_transmit(&msg, pdMS_TO_TICKS(100)); 
    if (err != ESP_OK) {
        goto error;
    } else {
        goto success;
    }

error:
    xSemaphoreGive(_mutex);
    ESP_LOGE(TAG, "Error in twai_transmit: %s", esp_err_to_name(err));
    return -1;
success:
    xSemaphoreGive(_mutex);
    ESP_LOGI(TAG, "WRITE - ID: %u | DATA:", msg.identifier);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, msg.data, msg.data_length_code, ESP_LOG_INFO);
    return 0;
}

/**
 * @brief Read CAN message 
 * 
 * @param frame
 * @param id identifier
 * @param length data length
 * @param timeout 
 * @return error: -1, succed: 0
 */
int BusCAN::read(Frame_t* frame, uint16_t* id, uint8_t* length, TickType_t timeout)
{
    esp_err_t err;
    twai_message_t msg;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    err = twai_receive(&msg, timeout);
    if (err != ESP_OK) {
        goto error;
    } else {
        memcpy(frame, msg.data, sizeof(Frame_t));
        *id = msg.identifier;
        *length = msg.data_length_code;
        goto success;
    }    

error:
    xSemaphoreGive(_mutex);
    ESP_LOGE(TAG, "Error in twai_receive: %s", esp_err_to_name(err));
    return -1;
success:
    xSemaphoreGive(_mutex);
    ESP_LOGI(TAG, "READ - ID: %u | DATA:", msg.identifier);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, msg.data, msg.data_length_code, ESP_LOG_INFO);
    return 0;
}