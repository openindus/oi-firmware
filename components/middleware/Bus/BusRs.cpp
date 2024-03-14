/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusRs.c
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BusRs.h"

#define BUS_RS_SYNC_BYTE 0xAA
#define BUS_RS_HEADER_LENGTH 10
#define BUS_RS_DATA_LENGTH_MAX 1024
#define BUS_RS_FRAME_LENGTH_MAX (BUS_RS_HEADER_LENGTH + BUS_RS_DATA_LENGTH_MAX)

static const char BUS_RS_TAG[] = "Bus RS";

uart_port_t BusRs::_port;
QueueHandle_t BusRs::_eventQueue;
SemaphoreHandle_t BusRs::_mutex;
SemaphoreHandle_t BusRs::_semaphore;

/**
 * @brief initialization of RS communication
 * 
 * @param port Uart port num 
 * @param tx_num Tx gpio num
 * @param rx_num Rx gpio Num
 */
void BusRs::begin(uart_port_t port, gpio_num_t tx_num, gpio_num_t rx_num)
{
    _port = port;

    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    _semaphore = xSemaphoreCreateMutex();
    xSemaphoreGive(_semaphore);

    ESP_LOGI(BUS_RS_TAG, "Configure uart parameters");
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
    ESP_LOGI(BUS_RS_TAG, "Install uart driver");
    if (uart_is_driver_installed(_port)) {
        uart_driver_delete(_port);
    }
    ESP_ERROR_CHECK(uart_driver_install(_port, BUS_RS_FRAME_LENGTH_MAX*2, 
        BUS_RS_FRAME_LENGTH_MAX*2, 10, &_eventQueue, 0));

    ESP_ERROR_CHECK(uart_set_pin(_port, tx_num, rx_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // esp_log_level_set(BUS_RS_TAG, ESP_LOG_WARN);
}

/**
 * @brief deinitialization of RS communication
 */
void BusRs::end(void)
{
    ESP_LOGI(BUS_RS_TAG, "Uninstall uart driver");
    ESP_ERROR_CHECK(uart_driver_delete(_port));
}

/**
 * @brief Send RS frame
 * 
 * @param frame 
 */
void BusRs::write(Frame_t* frame, TickType_t timeout)
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
    ESP_LOGI(BUS_RS_TAG, "WRITE - ID: %u | CMD: 0x%02X | SEQ: 0x%02X | CHKSUM: 0x%02X | DATA:", \
            frame->identifier, frame->command, frame->seq_num, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(BUS_RS_TAG, frame->data, frame->length, ESP_LOG_INFO);
}

/**
 * @brief Receive RS frame
 * 
 * @param frame 
 */
int BusRs::read(Frame_t* frame, TickType_t timeout)
{
    uart_event_t event;
    uint8_t* buffer = (uint8_t*)malloc(BUS_RS_FRAME_LENGTH_MAX);
    int index = 0;
    while (1) {
        if (xQueueReceive(_eventQueue, (void*)&event, timeout) == pdTRUE) {
            if (event.type == UART_DATA) {
                uart_read_bytes(_port, buffer, event.size, timeout);
                if (buffer[0] == BUS_RS_SYNC_BYTE && event.size >= BUS_RS_HEADER_LENGTH && index == 0) {
                    memcpy(frame, buffer, BUS_RS_HEADER_LENGTH);
                    memcpy(frame->data, &buffer[BUS_RS_HEADER_LENGTH], event.size - BUS_RS_HEADER_LENGTH);
                    index += (event.size - BUS_RS_HEADER_LENGTH);
                } else {
                    memcpy(&frame->data[index], buffer, event.size);
                    index += event.size;
                }
                if (index >= frame->length || index > BUS_RS_FRAME_LENGTH_MAX) {
                    xSemaphoreGive(_semaphore);
                    if (_verifyChecksum(frame)) {
                        goto succeed;
                    } else {
                        ESP_LOGE(BUS_RS_TAG, "Invalid checksum");
                        goto error;
                    }
                }
            } else {
                uart_flush_input(_port);
                xQueueReset(_eventQueue);
                ESP_LOGE(BUS_RS_TAG, "Event type error: %d", event.type);
                goto error;
            }
        } else {
            ESP_LOGE(BUS_RS_TAG, "Timeout error");
            goto error;
        }
    }
error:
    free(buffer);
    buffer = NULL;
    xSemaphoreGive(_semaphore);
    return -1;
succeed:
    free(buffer);
    buffer = NULL;
    xSemaphoreGive(_semaphore);
    ESP_LOGI(BUS_RS_TAG, "RECV - ID: %u | CMD: 0x%02X | SEQ: 0x%02X | CHKSUM: 0x%02X | DATA:", \
            frame->identifier, frame->command, frame->seq_num, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(BUS_RS_TAG, frame->data, frame->length, ESP_LOG_INFO);
    return 0;
}

/**
 * @brief Request RS frame
 * 
 * @param frame 
 * @param timeout 
 * @return int 
 */
int BusRs::requestFrom(Frame_t* frame, TickType_t timeout)
{
    write(frame, timeout);
    return read(frame, timeout);
}

/**
 * @brief Calculates the checksum of a RS frame
 * 
 * @param frame 
 * @return uint8_t 
 */
uint8_t BusRs::_calculateChecksum(Frame_t *frame)
{
    uint8_t checksum = 0xFE;
    checksum ^= frame->command; 
    checksum ^= (frame->identifier >> 8);
    checksum ^= (frame->identifier & 0xFF);
    checksum ^= (frame->flags >> 8);
    checksum ^= (frame->flags & 0xFF);
    checksum ^= (frame->length >> 8);
    checksum ^= (frame->length & 0xFF);
    checksum ^= frame->seq_num;
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
bool BusRs::_verifyChecksum(Frame_t *frame)
{
    uint8_t checksum = _calculateChecksum(frame);
    return (frame->checksum == checksum);
}
