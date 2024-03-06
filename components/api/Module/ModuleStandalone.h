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

class ModuleStandalone
{
public:

    static void init(void);

    static void restart(void);
    static void ledOn(LedColor_t color);
    static void ledOff(void);
    static void ledBlink(LedColor_t color, uint32_t period);
    static float getTemperature(void);
    static int getBoardType(void);
    static void setBoardType(int boardType);
    static int getSerialNum(void);
    static void setSerialNum(int serialNum);

private:

    static int32_t _getBoardInfoFromNVS(const char* key);
    static void _setBoardInfoToNVS(const char* key, int32_t value);
};
