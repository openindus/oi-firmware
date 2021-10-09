/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIWire.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// #define I2C_BUFFER_LENGTH 128

// class OIWire
// {

// public:
//     OIWire();
//     ~OIWire();
    
//     bool begin(int sda=-1, int scl=-1, uint32_t frequency=0);

//     void setClock(uint32_t frequency);
//     size_t getClock();

//     void beginTransmission(uint16_t address);
//     void beginTransmission(uint8_t address);
//     void beginTransmission(int address);

//     uint8_t endTransmission(bool sendStop);
//     uint8_t endTransmission(void);

//     uint8_t requestFrom(uint16_t address, uint8_t size, bool sendStop);
//     uint8_t requestFrom(uint16_t address, uint8_t size, uint8_t sendStop);
//     uint8_t requestFrom(uint16_t address, uint8_t size);
//     uint8_t requestFrom(uint8_t address, uint8_t size, uint8_t sendStop);
//     uint8_t requestFrom(uint8_t address, uint8_t size);
//     uint8_t requestFrom(int address, int size, int sendStop);
//     uint8_t requestFrom(int address, int size);

//     size_t write(uint8_t);
//     size_t write(const uint8_t *, size_t);
//     int available(void);
//     int read(void);
//     int peek(void);
//     void flush(void);

//     inline size_t write(const char * s)
//     {
//         return write((uint8_t*) s, strlen(s));
//     }
//     inline size_t write(unsigned long n)
//     {
//         return write((uint8_t)n);
//     }
//     inline size_t write(long n)
//     {
//         return write((uint8_t)n);
//     }
//     inline size_t write(unsigned int n)
//     {
//         return write((uint8_t)n);
//     }
//     inline size_t write(int n)
//     {
//         return write((uint8_t)n);
//     }

//     void onReceive( void (*)(int) );
//     void onRequest( void (*)(void) );
// };

// extern OIWire Wire;

// #define TwoWire OIWire