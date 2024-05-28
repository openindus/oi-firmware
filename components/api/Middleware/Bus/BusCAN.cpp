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
int BusCAN::begin(gpio_num_t txNum, gpio_num_t rxNum)
{
    int err = 0;

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
    err |= twai_driver_install(&g_config, &t_config, &f_config);

    /* Start TWAI driver */
    ESP_LOGI(TAG, "start twai driver");
    err |= twai_start();

    return err;
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
 * @param size frame size 
 * @return error: -1, succeed: 0
 */
int BusCAN::write(Frame_t* frame, uint16_t id, uint8_t size)
{
    esp_err_t err;
    twai_message_t msg = {
        .flags = 0,
        .identifier = id,
        .data_length_code = size,
        .data = {}
    };
    memcpy(msg.data, frame, size);
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
#if defined(DEBUG_BUS)    
    ESP_LOGI(TAG, "WRITE - ID: %u | DATA:", msg.identifier);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, msg.data, msg.data_length_code, ESP_LOG_INFO);
#endif
    return 0;
}

/**
 * @brief Read CAN message 
 * 
 * @param frame
 * @param id identifier
 * @param size frame size
 * @param timeout 
 * @return error: -1, succed: 0
 */
int BusCAN::read(Frame_t* frame, uint16_t* id, uint8_t* size, TickType_t timeout)
{
    esp_err_t err;
    twai_message_t msg;
    // xSemaphoreTake(_mutex, portMAX_DELAY);
    err = twai_receive(&msg, timeout);
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (err != ESP_OK) {
        goto error;
    } else {
        memcpy(frame, msg.data, sizeof(Frame_t));
        *id = msg.identifier;
        *size = msg.data_length_code;
        goto success;
    }    

error:
    xSemaphoreGive(_mutex);    
    ESP_LOGE(TAG, "Error in twai_receive: %s", esp_err_to_name(err));
    return -1;
success:
    xSemaphoreGive(_mutex);
#if defined(DEBUG_BUS)    
    ESP_LOGI(TAG, "READ - ID: %u | DATA:", msg.identifier);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, msg.data, msg.data_length_code, ESP_LOG_INFO);
#endif
    return 0;
}