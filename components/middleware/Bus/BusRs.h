/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusRs.h
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

class BusRs
{
public:

    typedef struct __attribute__((__packed__)) {
        uint8_t sync;
        uint8_t command;
        uint16_t identifier;
        union {
            struct { 
                uint16_t broadcast  : 1;
                uint16_t direction  : 1;
                uint16_t ack        : 1;
                uint16_t status     : 1;
                uint16_t error      : 8;
                uint16_t reserved   : 4;
            };
            uint16_t flags;
        };
        uint16_t length;
        uint8_t seq_num;
        uint8_t checksum;
        uint8_t* data;
    } Frame_t;

    static void begin(uart_port_t port, gpio_num_t tx_num, gpio_num_t rx_num);
    static void end(void);
    static void write(Frame_t* frame, TickType_t timeout=0);
    static int read(Frame_t* frame, TickType_t timeout=portMAX_DELAY);
    static int requestFrom(Frame_t* frame, TickType_t timeout=portMAX_DELAY);

private:

    static uart_port_t _port;
    static QueueHandle_t _eventQueue;
    static SemaphoreHandle_t _mutex;
    static SemaphoreHandle_t _semaphore;

    static uint8_t _calculateChecksum(Frame_t *frame);
    static bool _verifyChecksum(Frame_t *frame);

};
