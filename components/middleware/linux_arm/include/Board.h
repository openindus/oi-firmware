/**
 * @file Board.h
 * @brief 
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>

class Board
{
public:

    static void restart(void);
    static float getTemperature(void);    
    static uint8_t getBoardType(void);
    static uint8_t getHardwareVariant(void);
    static uint32_t getSerialNum(void);
    static int64_t getTimestamp(void);
    static void getSoftwareVersion(char software_version[32]);
    static bool setBoardInfo(uint8_t board_type, uint8_t hardware_variant, uint32_t serial_num, int64_t timestamp);

};