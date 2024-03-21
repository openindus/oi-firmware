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

/* Struct can fill an eFuse block of 32 bytes */
typedef struct {
    char board_type[16];
    int serial_number;
    char hardware_version[12];
} Module_eFuse_Info_t;

typedef struct {
    Module_eFuse_Info_t efuse;
    char software_version[32];
} Module_Info_t;

class ModuleStandalone
{
public:

    static void init(void);

    static void restart(void);
    static void ledOn(LedColor_t color);
    static void ledOff(void);
    static void ledBlink(LedColor_t color, uint32_t period);
    static float getTemperature(void);
    static void getBoardType(char* board_type);
    static int getSerialNum(void);
    static void getHardwareVersion(char* version);
    static void getSoftwareVersion(char* version);
    static bool setBoardInfo(char* board_type, int serial_num, char* hardware_version);


private:

    static int32_t _getBoardInfoFromNVS(const char* key);
    static void _setBoardInfoToNVS(const char* key, int32_t value);
};
