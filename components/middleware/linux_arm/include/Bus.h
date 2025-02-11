/**
 * @file Bus.h
 * @brief 
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>

class Bus {};

class BusIO
{
private:

public:
    BusIO();
    ~BusIO();

    static void writeSync(uint8_t sync);
    static uint8_t readSync(void);

};

class BusRS 
{
private:

public:
    BusRS();
    ~BusRS();

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

    static int read(Frame_t *frame, uint32_t timeout);
    static int write(Frame_t *frame, uint32_t timeout=0);
    static int transfer(Frame_t *frame, uint32_t timeout);
};

class BusCAN
{
private:

public:
    BusCAN();
    ~BusCAN();

    typedef struct __attribute__((__packed__)) {
        union {
            struct {
                uint8_t cmd;
                uint8_t args[7];
            };
            uint8_t data[8];
        };

    } Frame_t;

    static int read(Frame_t* frame, uint16_t* id, uint8_t* size, uint32_t timeout=0);
    static int write(Frame_t* frame, uint16_t id, uint8_t size=8);
};