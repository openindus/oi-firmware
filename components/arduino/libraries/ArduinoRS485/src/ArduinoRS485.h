/**
 * @file ArduinoRS485.h
 * @brief ArduinoRS485 library for OpenIndus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <Arduino.h>
#include "OpenIndus.h"

/**
 * @brief Arduino RS485 class
 */
class RS485Class : public RS
{
public:
    RS485Class(const RS& rs) : RS(rs) {}

    void begin(unsigned long baudrate, uint16_t config) {
        RS::begin(baudrate, static_cast<Config>(config), RS::Mode::RS485);
    }

    int read(void) { return RS::read(); }
    size_t readBytes(uint8_t *buffer, int size) { return RS::read(buffer, size);}

    size_t write(const uint8_t* buffer, size_t size) { return RS::write(buffer, size); }
    size_t write(const char * s) { return RS::write((uint8_t*) s, strlen(s)); }
    size_t write(unsigned long n) { return RS::write((uint8_t*)&n, sizeof(unsigned long)); }
    size_t write(long n) { return RS::write((uint8_t*)&n, sizeof(long)); }
    size_t write(unsigned int n) { return RS::write((uint8_t*)&n, sizeof(unsigned int)); }
    size_t write(int n) { return RS::write((uint8_t*)&n, sizeof(int)); }

    void receive(void) {}
    void noReceive(void) {}
    void beginTransmission(void) {}
    void endTransmission(void) {}
};

extern RS485Class RS485;