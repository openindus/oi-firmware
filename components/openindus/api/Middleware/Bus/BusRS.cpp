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
SemaphoreHandle_t BusRS::_writeMutex;
SemaphoreHandle_t BusRS::_writeReadMutex;
uint8_t BusRS::_rxBuffer[128];
size_t BusRS::_rxLength = 0;
size_t BusRS::_rxIndex = 0;
size_t BusRS::_rxPending = 0;

/**
 * @brief initialization of RS communication
 * 
 * @param port Uart port num 
 * @param tx_num Tx gpio num
 * @param rx_num Rx gpio Num
 */
int BusRS::begin(uart_port_t port, gpio_num_t tx_num, gpio_num_t rx_num)
{
    int err = 0;
    _port = port;

    /* Mutex */
    _writeMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_writeMutex);
    _writeReadMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_writeReadMutex);

    ESP_LOGI(TAG, "Configure uart parameters");
    uart_config_t uart_config = {
        .baud_rate = 921600UL,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122U,
        .source_clk = UART_SCLK_APB,
        .flags = {
            .allow_pd = 0,
            .backup_before_sleep = 0
        }
    };
    err |= uart_param_config( _port, &uart_config);

    /* install UART driver, and get the queue */
    ESP_LOGI(TAG, "Install uart driver");
    if (uart_is_driver_installed(_port)) {
        uart_driver_delete(_port);
    }
    err |= uart_driver_install(_port, BUS_RS_FRAME_LENGTH_MAX*2, BUS_RS_FRAME_LENGTH_MAX*2, 10, &_eventQueue, 0);

    err |= uart_set_pin(_port, tx_num, rx_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    /*
     * Do not generate an event after a single byte time of inactivity.  At
     * 921600 baud this made a normal frame arrive in several UART_DATA events
     * very frequently.  read() supports fragmented frames, but a slightly
     * longer timeout also avoids needless task wakeups.
     */
    err |= uart_set_rx_timeout(_port, 10);

    _rxLength = 0;
    _rxIndex = 0;
    _rxPending = 0;

    return err;
}

/**
 * @brief deinitialization of RS communication
 */
void BusRS::end(void)
{
    ESP_LOGI(TAG, "Uninstall uart driver");
    uart_driver_delete(_port);
}

/**
 * @brief Send RS frame
 * 
 * @param frame 
 */
void BusRS::write(Frame_t* frame, uint32_t timeout)
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
            xSemaphoreTake(_writeReadMutex, portMAX_DELAY);
        xSemaphoreTake(_writeMutex, portMAX_DELAY);
        uart_write_bytes(_port, (const char*) buffer, length);
        uart_wait_tx_done(_port, pdMS_TO_TICKS(timeout));
        xSemaphoreGive(_writeMutex);
    }
    free(buffer);
    buffer = NULL;
#if defined(DEBUG_BUS)
    ESP_LOGI(TAG, "WRITE - ID: %u | CMD: 0x%02X | LENGTH: 0x%02X | CHCK: 0x%02X | DATA:", \
            frame->id, frame->cmd, frame->length, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, frame->data, frame->length, ESP_LOG_INFO);
#endif
}

/**
 * @brief Receive RS frame
 * 
 * @param frame 
 */
int BusRS::read(Frame_t* frame, uint32_t timeout)
{
    uart_event_t event;
    uint8_t header[BUS_RS_HEADER_LENGTH];
    size_t headerLength = 0;
    size_t dataLength = 0;

    while (1) {
        /* Bytes left over from a previous call come first: a burst of frames is
         * often merged into a single UART event, and everything after the frame
         * returned back then is still waiting here. */
        while (_rxIndex < _rxLength) {
            uint8_t byte = _rxBuffer[_rxIndex++];

            if (headerLength < BUS_RS_HEADER_LENGTH) {
                header[headerLength++] = byte;
                if (headerLength != BUS_RS_HEADER_LENGTH) {
                    continue;
                }

                /* Frame_t also contains a pointer; copy only its wire header. */
                memcpy(frame, header, BUS_RS_HEADER_LENGTH);
                if ((frame->sync != BUS_RS_SYNC_BYTE) || (frame->length > BUS_RS_DATA_LENGTH_MAX)) {
                    ESP_LOGE(TAG, "Invalid header frame");
                    goto error;
                }
                if (frame->length == 0) {
                    if (_verifyChecksum(frame)) {
                        goto success;
                    }
                    ESP_LOGE(TAG, "Invalid checksum: %02X, expected: %02X", frame->checksum, _calculateChecksum(frame));
                    goto error;
                }
                if (frame->data == NULL) {
                    ESP_LOGE(TAG, "No buffer provided for received frame");
                    goto error;
                }
                continue;
            }

            frame->data[dataLength++] = byte;
            if (dataLength == frame->length) {
                if (_verifyChecksum(frame)) {
                    goto success;
                }
                ESP_LOGE(TAG, "Invalid checksum: %02X, expected: %02X", frame->checksum, _calculateChecksum(frame));
                goto error;
            }
        }

        /* Then the rest of the event being processed, chunk by chunk. */
        if (_rxPending > 0) {
            size_t chunkLength = (_rxPending > sizeof(_rxBuffer)) ? sizeof(_rxBuffer) : _rxPending;
            int bytesRead = uart_read_bytes(_port, _rxBuffer, chunkLength, pdMS_TO_TICKS(timeout));
            if (bytesRead <= 0) {
                ESP_LOGE(TAG, "UART data event without data");
                goto error;
            }
            _rxPending -= bytesRead;
            _rxIndex = 0;
            _rxLength = bytesRead;
            continue;
        }

        /* Nothing buffered anymore: wait for the next event. */
        if (xQueueReceive(_eventQueue, (void*)&event, pdMS_TO_TICKS(timeout)) == pdTRUE) {
            if (event.type == UART_DATA) {
                _rxPending = event.size;
            } else {
                ESP_LOGE(TAG, "Event type error: %d", event.type);
                goto error;
            }
        } else {
            ESP_LOGE(TAG, "Timeout error");
            goto error;
        }
    }
error:
    /* The bytes of the partial frame are gone, so the stream is desynchronized:
     * drop what is buffered instead of parsing the middle of a frame as a header. */
    _flushRx();
    xSemaphoreGive(_writeReadMutex);
    return -1;
success:
    xSemaphoreGive(_writeReadMutex);
#if defined(DEBUG_BUS)
    ESP_LOGI(TAG, "READ - ID: %u | CMD: 0x%02X | LENGTH: 0x%02X | CHCK: 0x%02X | DATA:", \
            frame->id, frame->cmd, frame->length, frame->checksum);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, frame->data, frame->length, ESP_LOG_INFO);
#endif
    return 0;
}

/**
 * @brief Discard the buffered rx stream and resynchronize on the next frame
 */
void BusRS::_flushRx(void)
{
    uart_flush_input(_port);
    xQueueReset(_eventQueue);
    _rxLength = 0;
    _rxIndex = 0;
    _rxPending = 0;
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
