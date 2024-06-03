/**
 * @file Board.h
 * @brief Board interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

/* Struct can fill an eFuse block of 32 msgBytes */
typedef struct __attribute__((__packed__)) {
    uint16_t board_type;
    uint32_t serial_number;
    char hardware_version[4]; // AA01
    int64_t timestamp; // Unix timestamp
    uint8_t reserved[13];
    uint8_t checksum;
} Board_eFuse_Info_t;

typedef struct {
    Board_eFuse_Info_t efuse;
    char software_version[32];
} Board_Info_t;

class Board
{
public:

    static int init(void);

    static void restart(void);
    static float getTemperature(void);
    static uint16_t getBoardType(void);
    static uint32_t getSerialNum(void);
    static void getHardwareVersion(char hardware_version[4]);
    static int64_t getTimestamp(void);
    static void getSoftwareVersion(char software_version[32]);
    static bool setBoardInfo(uint16_t board_type, uint32_t serial_num, char hardware_version[4], int64_t timestamp);

private:

    static uint8_t _calculate_eFuse_checksum(uint8_t* data);
    static bool _verify_eFuse_checksum(Board_eFuse_Info_t info);

};