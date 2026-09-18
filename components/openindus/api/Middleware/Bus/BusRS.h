/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusRS.h
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

class BusRS
{
public:

    typedef struct __attribute__((__packed__)) {
        uint8_t sync;
        uint8_t cmd;
        union {
            struct { 
                uint16_t id         : 11;   // 0 --> Broadcast ID
                uint16_t dir        : 1;    // 1 --> Master to Slave | 0 --> Slave to Master
                uint16_t ack        : 1;    // ack needed
                uint16_t error      : 1;    // Error
                uint16_t reserved   : 2;
            };
            uint16_t flags;
        };
        uint16_t length; // data length
        uint8_t checksum;
        uint8_t* data;
    } Frame_t;

    static int begin(uart_port_t port, gpio_num_t tx_num, gpio_num_t rx_num);
    static void end(void);
    static void write(Frame_t* frame, uint32_t timeout=0);
    static int read(Frame_t* frame, uint32_t timeout=portMAX_DELAY);

private:

    static uart_port_t _port;
    static QueueHandle_t _eventQueue;
    static SemaphoreHandle_t _writeMutex;
    static SemaphoreHandle_t _writeReadMutex;

    /* Rx stream state kept across read() calls: several frames can be received
     * in a single UART event, and the surplus must survive until the next call. */
    static uint8_t _rxBuffer[128];
    static size_t _rxLength;  // bytes held in _rxBuffer
    static size_t _rxIndex;   // next byte of _rxBuffer to consume
    static size_t _rxPending; // bytes announced by the current event, still in the driver

    static void _flushRx(void);
    static uint8_t _calculateChecksum(Frame_t *frame);
    static bool _verifyChecksum(Frame_t *frame);

};
