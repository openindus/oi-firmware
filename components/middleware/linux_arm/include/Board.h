/**
 * @file Board.h
 * @brief 
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>

typedef struct __attribute__((__packed__)) {
    uint16_t type;
    uint32_t serial_number;
    char name[32];
    char version[16];
    char date[16];
    char time[16];
} Board_Info_t;

class Board
{
private:

public:

    static void restart(void);

};

class BoardUtils
{
private:

public:
    static bool areTypeCompatible(uint16_t type1, uint16_t type2);
    static void typeToName(uint16_t type, char* name);
};