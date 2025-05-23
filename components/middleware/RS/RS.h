/**
 * @file RS.h
 * @brief RS485/232 bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include "sc16is750/sc16is750.h"

typedef enum {
    RS_485,
    RS_232
} RS_Mode_e;

class RS
{
public:

    inline RS(spi_host_device_t host, gpio_num_t cs, gpio_num_t intr) :
        _spi_host(host),
        _pin_cs(cs),
        _pin_intr(intr) {}

    void begin(unsigned long baudrate = 115200, uint16_t config = 0x001c, RS_Mode_e mode = RS_485);
    void end(void);
    int available(void);
    int availableForWrite(void);
    int read(void);
    size_t read(uint8_t *buffer, size_t size);
    size_t write(uint8_t byte);
    size_t write(const uint8_t* buffer, size_t size);

    inline size_t read(char * buffer, size_t size) {
        return read((uint8_t*) buffer, size);
    }

    inline size_t write(const char * buffer, size_t size) {
        return write((uint8_t*) buffer, size);
    }

    inline size_t write(const char * s) {
        return write((uint8_t*) s, strlen(s));
    }

    inline size_t write(unsigned long n) {
        return write((uint8_t) n);
    }

    inline size_t write(long n) {
        return write((uint8_t) n);
    }

    inline size_t write(unsigned int n) {
        return write((uint8_t) n);
    }

    inline size_t write(int n) {
        return write((uint8_t) n);
    }

    inline void noReceive(void) {}
    inline void beginTransmission(void) {}
    inline void endTransmission(void) {}
    inline void receive(void) {}

    inline size_t readBytes(uint8_t *buffer, int size) { return read(buffer, size); }

private:

    spi_host_device_t _spi_host; 
    gpio_num_t _pin_cs; 
    gpio_num_t _pin_intr;
    RS_Mode_e _mode;
};