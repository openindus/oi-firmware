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

    typedef enum {
        CORE            = 0,
        CORE_LITE       = 1,
        DISCRETE        = 2,
        DISCRETE_VE     = 3,
        MIXED           = 4,
        RELAY_LP        = 5,
        RELAY_HP        = 6,
        STEPPER         = 7,
        STEPPER_VE      = 8,
        BRUSHLESS       = 9,
        ANALOGLS        = 10
    } BoardType_t;

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

#define MODULE_PROGRAMMING()    ModuleStandalone::ledBlink(LED_WHITE, 1000)
#define MODULE_INITIALIZED()    ModuleStandalone::ledBlink(LED_BLUE, 1000)
#define MODULE_ERROR()          ModuleStandalone::ledBlink(LED_RED, 1000)
#define MODULE_PAIRED()         ModuleStandalone::ledBlink(LED_GREEN, 1000)
