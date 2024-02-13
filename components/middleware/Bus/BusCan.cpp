/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCan.c
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BusCan.h"

static const char BUS_TWAI_TAG[] = "Bus TWAI";

SemaphoreHandle_t BusCan::_mutex;

/**
 * @brief initialization of CAN communication
 * 
 * @param txNum 
 * @param rxNum 
 */
void BusCan::begin(gpio_num_t txNum, gpio_num_t rxNum)
{
    ESP_LOGV(BUS_TWAI_TAG, "init");

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
    ESP_LOGI(BUS_TWAI_TAG, "install twai driver");
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));

    /* Start TWAI driver */
    ESP_LOGI(BUS_TWAI_TAG, "start twai driver");
    ESP_ERROR_CHECK(twai_start());

    esp_log_level_set(BUS_TWAI_TAG, ESP_LOG_WARN);
}

/**
 * @brief deinitialization of CAN communication
 */
void BusCan::end(void)
{
    ESP_LOGV(BUS_TWAI_TAG, "deinit");

    /* Stop TWAI driver */
    ESP_LOGI(BUS_TWAI_TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_stop());

    /* uninstall twai driver */
    ESP_LOGI(BUS_TWAI_TAG, "stop twai driver");
    ESP_ERROR_CHECK(twai_driver_uninstall());
}

/**
 * @brief Write message on the CAN bus
 * 
 * @param frame 
 * @param id 
 * @return int 
 */
int BusCan::write(Frame_t* frame, uint16_t id)
{
    twai_message_t msg = {
        .flags = 0,
        .identifier = id,
        .data_length_code = 8,
        .data = {}
    };
    memcpy(msg.data, frame, 8);
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (twai_transmit(&msg, pdMS_TO_TICKS(100)) != ESP_OK) {
        goto error;
    } else {
        goto succeed;
    }

error:
    xSemaphoreGive(_mutex);
    return -1;
succeed:
    xSemaphoreGive(_mutex);
    ESP_LOGI(BUS_TWAI_TAG, "CMD: 0x%02X | TYPE: 0x%02X | DATA: 0x%08X", frame->command, frame->type, frame->data);
    return 0;
}

/**
 * @brief Read CAN message 
 * 
 * @param frame
 * @param id 
 * @param timeout 
 * @return int 
 */
int BusCan::read(Frame_t* frame, uint16_t* id, TickType_t timeout)
{
    twai_message_t msg;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (twai_receive(&msg, timeout) != ESP_OK) {
        goto error;
    } else {
        memcpy(frame, msg.data, sizeof(Frame_t));
        *id = (uint16_t)msg.identifier;
        goto succeed;
    }    

error:
    xSemaphoreGive(_mutex);
    return -1;
succeed:
    xSemaphoreGive(_mutex);
    ESP_LOGI(BUS_TWAI_TAG, "CMD: 0x%02X | TYPE: 0x%02X | DATA: 0x%08X", frame->command, frame->type, frame->data);
    return 0;
}