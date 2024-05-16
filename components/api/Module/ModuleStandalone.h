/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleStandalone.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

/* Struct can fill an eFuse block of 32 msgBytes */
typedef struct __attribute__((__packed__)) {
    uint16_t board_type;
    uint32_t serial_number;
    char hardware_version[4]; // AA01
    char date_code[4]; // MMYY
    uint8_t reserved[17];
    uint8_t checksum;
} Module_eFuse_Info_t;

typedef struct {
    Module_eFuse_Info_t efuse;
    char software_version[32];
} Module_Info_t;

class ModuleStandalone
{
public:

    static int init(void);

    static void restart(void);
    static void ledOn(LedColor_t color);
    static void ledOff(void);
    static void ledBlink(LedColor_t color, uint32_t period);
    static float getTemperature(void);
    static uint16_t getBoardType(void);
    static uint32_t getSerialNum(void);
    static void getHardwareVersion(char hardware_version[4]);
    static void getDateCode(char date_code[4]);
    static void getSoftwareVersion(char software_version[32]);
    static bool setBoardInfo(uint16_t board_type, uint32_t serial_num, char hardware_version[4], char date_code[4]);

private:

    static uint8_t _calculate_eFuse_checksum(uint8_t* data);
    static bool _verify_eFuse_checksum(Module_eFuse_Info_t info);

};
