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

class BusRS
{
public:

    typedef struct __attribute__((__packed__)) {
        uint8_t sync;
        uint8_t cmd;
        union {
            struct { 
                uint16_t id         : 11;   // 0 --> Broadcast ID
                uint16_t dir        : 1;    // 1 --> ControllerMaster to ControllerSlave | 0 --> ControllerSlave to ControllerMaster
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
    static void write(Frame_t* frame, TickType_t timeout=0);
    static int read(Frame_t* frame, TickType_t timeout=portMAX_DELAY);
    static int transfer(Frame_t* frame, TickType_t timeout);

private:

    static uart_port_t _port;
    static QueueHandle_t _eventQueue;
    static SemaphoreHandle_t _writeMutex;
    static SemaphoreHandle_t _writeReadMutex;
    static SemaphoreHandle_t _transferMutex;

    static uint8_t _calculateChecksum(Frame_t *frame);
    static bool _verifyChecksum(Frame_t *frame);

};
