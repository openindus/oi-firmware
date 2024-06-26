/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusRS.c
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BusRS.h"

#define BUS_RS_SYNC_BYTE 0xAA
#define BUS_RS_HEADER_LENGTH 7
#define BUS_RS_DATA_LENGTH_MAX 1024
#define BUS_RS_FRAME_LENGTH_MAX (BUS_RS_HEADER_LENGTH + BUS_RS_DATA_LENGTH_MAX)

static const char TAG[] = "BusRS";

uart_port_t BusRS::_port;
QueueHandle_t BusRS::_eventQueue;
SemaphoreHandle_t BusRS::_mutex;
SemaphoreHandle_t BusRS::_semaphore;

/**
 * @brief initialization of RS communication
 * 
 * @param port Uart port num 
 * @param tx_num Tx gpio num
 * @param rx_num Rx gpio Num
 */
void BusRS::begin(uart_port_t port, gpio_num_t tx_num, gpio_num_t rx_num)
{
    _port = port;

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    _semaphore = xSemaphoreCreateMutex();
    xSemaphoreGive(_semaphore);

    ESP_LOGI(TAG, "Configure uart parameters");
    uart_config_t uart_config = {
        .baud_rate = 921600UL,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122U,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config( _port, &uart_config));

    /* install UART driver, and get the queue */
    ESP_LOGI(TAG, "Install uart driver");
    if (uart_is_driver_installed(_port)) {
        uart_driver_delete(_port);
    }
    ESP_ERROR_CHECK(uart_driver_install(_port, BUS_RS_FRAME_LENGTH_MAX*2, 
        BUS_RS_FRAME_LENGTH_MAX*2, 10, &_eventQueue, 0));

    ESP_ERROR_CHECK(uart_set_pin(_port, tx_num, rx_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

#if !defined(DEBUG_BUS)
    esp_log_level_set(TAG, ESP_LOG_WARN);
#endif

}

/**
 * @brief deinitialization of RS communication
 */
void BusRS::end(void)
{
    ESP_LOGI(TAG, "Uninstall uart driver");
    ESP_ERROR_CHECK(uart_driver_delete(_port));
}

/**
 * @brief Send RS frame
 * 
 * @param frame 
 */
void BusRS::write(Frame_t* frame, TickType_t timeout)
{
    uint8_t* buffer = (uint8_t*)malloc(BUS_RS_FRAME_LENGTH_MAX);
    size_t length;
    frame->sync = BUS_RS_SYNC_BYTE;
    if (frame->length <= BUS_RS_DATA_LENGTH_MAX) {
        frame->checksum = _calculateChecksum(frame);
        memcpy(buffer, frame, BUS_RS_HEADER_LENGTH);
        memcpy(&buffer[BUS_RS_HEADER_LENGTH], frame->data, frame->length);
        length = frame->length + BUS_RS_HEADER_LENGTH;
        if(frame->ack)
            xSemaphoreTake(_semaphore, portMAX_DELAY);
        xSemaphoreTake(_mutex, portMAX_DELAY);
        uart_write_bytes(_port, (const char*) buffer, length);
        uart_wait_tx_done(_port, timeout);
        xSemaphoreGive(_mutex);
    }
    free(buffer);
    buffer = NULL;
    ESP_LOGI(TAG, "WRITE - ID: %u | CMD: 0x%02X | CHKSUM: 0x%02X | DATA:", \
        frame->id, frame->cmd, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, frame->data, frame->length, ESP_LOG_INFO);
}

/**
 * @brief Receive RS frame
 * 
 * @param frame 
 */
int BusRS::read(Frame_t* frame, TickType_t timeout)
{
    uart_event_t event;
    uint8_t* buffer = (uint8_t*)malloc(BUS_RS_FRAME_LENGTH_MAX);
    int index = 0;
    while (1) {
        if (xQueueReceive(_eventQueue, (void*)&event, timeout) == pdTRUE) {
            if (event.type == UART_DATA) {
                uart_read_bytes(_port, buffer, event.size, timeout);
                if (index == 0) { // Start frame
                    if (event.size >= BUS_RS_HEADER_LENGTH) { // Get header frame
                        memcpy(frame, buffer, BUS_RS_HEADER_LENGTH);
                        memcpy(frame->data, &buffer[BUS_RS_HEADER_LENGTH], event.size - BUS_RS_HEADER_LENGTH);
                        index += (event.size - BUS_RS_HEADER_LENGTH);
                        if ((frame->sync != BUS_RS_SYNC_BYTE) || 
                            (frame->length > BUS_RS_DATA_LENGTH_MAX)) { // Check header frame
                                ESP_LOGE(TAG, "Invalid header frame");
                                goto error;
                        }
                    } else {
                        // Ignore data
                    }
                } else {
                    memcpy(&frame->data[index], buffer, event.size);
                    index += event.size;
                }
                if (index >= frame->length || index > BUS_RS_FRAME_LENGTH_MAX) {
                    xSemaphoreGive(_semaphore);
                    if (_verifyChecksum(frame)) {
                        goto success;
                    } else {
                        ESP_LOGE(TAG, "Invalid checksum");
                        goto error;
                    }
                }
            } else {
                uart_flush_input(_port);
                xQueueReset(_eventQueue);
                ESP_LOGE(TAG, "Event type error: %d", event.type);
                goto error;
            }
        } else {
            ESP_LOGE(TAG, "Timeout error");
            goto error;
        }
    }
error:
    free(buffer);
    buffer = NULL;
    xSemaphoreGive(_semaphore);
    return -1;
success:
    free(buffer);
    buffer = NULL;
    xSemaphoreGive(_semaphore);
    ESP_LOGI(TAG, "READ - ID: %u | CMD: 0x%02X | CHKSUM: 0x%02X | DATA:", \
        frame->id, frame->cmd, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, frame->data, frame->length, ESP_LOG_INFO);
    return 0;
}

/**
 * @brief Calculates the checksum of a RS frame
 * 
 * @param frame 
 * @return uint8_t checksum
 */
uint8_t BusRS::_calculateChecksum(Frame_t *frame)
{
    uint8_t checksum = 0xFE;
    checksum ^= frame->cmd; 
    checksum ^= (frame->flags >> 8);
    checksum ^= (frame->flags & 0xFF);
    checksum ^= (frame->length >> 8);
    checksum ^= (frame->length & 0xFF);
    for (int i=0; i<frame->length; i++) {
        checksum ^= frame->data[i];
    }
    return checksum;
}

/**
 * @brief Verifies the checksum of an RS frame
 * 
 * @param frame 
 * @return true 
 * @return false 
 */
bool BusRS::_verifyChecksum(Frame_t *frame)
{
    uint8_t checksum = _calculateChecksum(frame);
    return (frame->checksum == checksum);
}
