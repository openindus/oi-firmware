/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file UsbSerial.h
 * @brief OpenIndus USB Serial
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

class UsbSerial
{
public:

    static void begin(int baudrate=115200);
    static void read(void* buf, uint32_t length, TickType_t timeout=portMAX_DELAY);
    static void write(const void* src, size_t size);
    static void flush(void);

protected:

    static QueueHandle_t _eventQueue;

};

/* Used for Esptool protocol */
class UsbSerialProtocol : public UsbSerial
{
public:

    enum {
        FLASH_LOADER_BEGIN         = 0x02,
        FLASH_LOADER_DATA          = 0x03,
        FLASH_LOADER_END           = 0x04,
        MEM_BEGIN           = 0x05,
        MEM_END             = 0x06,
        MEM_DATA            = 0x07,
        SYNC                = 0x08,
        WRITE_REG           = 0x09,
        READ_REG            = 0x0a,
        SPI_SET_PARAMS      = 0x0b,
        SPI_ATTACH          = 0x0d,
        CHANGE_BAUDRATE     = 0x0f,
        FLASH_LOADER_DEFL_BEGIN    = 0x10,
        FLASH_LOADER_DEFL_DATA     = 0x11,
        FLASH_LOADER_DEFL_END      = 0x12,
        SPI_FLASH_LOADER_MD5       = 0x13,
        GET_SECURITY_INFO   = 0x14,
    };

    typedef struct {
        uint8_t direction;
        uint8_t command;
        uint16_t size;
        union {
            uint32_t checksum; // For command packet
            uint32_t value; // For response packet
        };
        uint8_t* data;
    } Packet_t;

    static void begin(int baudrate=115200);
    static int read(Packet_t* packet, TickType_t timeout=portMAX_DELAY);
    static void write(Packet_t* packet);

};
