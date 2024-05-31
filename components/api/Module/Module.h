/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Module.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"
#include "ModulePinout.h"
#include "Led.h"

typedef enum {
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
} Module_State_t;

enum ModuleType_List_e {
    TYPE_OI_CORE            = (uint16_t) 3,
    TYPE_OI_CORELITE        = (uint16_t) 4,
    TYPE_OI_DISCRETE        = (uint16_t) 6,
    TYPE_OI_DISCRETE_VE     = (uint16_t) 7,
    TYPE_OI_MIXED           = (uint16_t) 8,
    TYPE_OI_RELAY_LP        = (uint16_t) 9,
    TYPE_OI_RELAY_HP        = (uint16_t) 10,
    TYPE_OI_STEPPER         = (uint16_t) 11,
    TYPE_OI_STEPPER_VE      = (uint16_t) 12,
    TYPE_OI_ANALOG_LS       = (uint16_t) 13,
    TYPE_OI_BRUSHLESS       = (uint16_t) 18,
};

class ModuleUtils
{
public:

    static inline char* typeToName(uint16_t type, char* name) {
        switch (type) {
            case TYPE_OI_CORE:          strcpy(name, "OICore"); break;
            case TYPE_OI_CORELITE:      strcpy(name, "OICoreLite"); break;
            case TYPE_OI_DISCRETE:      strcpy(name, "OIDiscrete"); break;
            case TYPE_OI_DISCRETE_VE:   strcpy(name, "OIDiscreteVE"); break;
            case TYPE_OI_MIXED:         strcpy(name, "OIMixed");    break;
            case TYPE_OI_RELAY_LP:      strcpy(name, "OIRelayLP"); break;
            case TYPE_OI_RELAY_HP:      strcpy(name, "OIRelayHP"); break;
            case TYPE_OI_STEPPER:       strcpy(name, "OIStepper"); break;
            case TYPE_OI_STEPPER_VE:    strcpy(name, "OIStepperVE"); break;
            case TYPE_OI_ANALOG_LS:     strcpy(name, "OIAnalogLS"); break;
            case TYPE_OI_BRUSHLESS:     strcpy(name, "OIBrushless"); break;
            default:                    strcpy(name, "Unknow"); break;
        }
        return name;
    }
};

/* Struct can fill an eFuse block of 32 msgBytes */
typedef struct __attribute__((__packed__)) {
    uint16_t board_type;
    uint32_t serial_number;
    char hardware_version[4]; // AA01
    int64_t timestamp; // Unix timestamp
    uint8_t reserved[13];
    uint8_t checksum;
} Module_eFuse_Info_t;

typedef struct {
    Module_eFuse_Info_t efuse;
    char software_version[32];
} Module_Info_t;

class ModuleStandalone
{
public:

    static int init(uint16_t type);

    static void restart(void);
    static void ledOn(LedColor_t color);
    static void ledOff(void);
    static void ledBlink(LedColor_t color, uint32_t period);
    static float getTemperature(void);
    static uint16_t getBoardType(void);
    static uint32_t getSerialNum(void);
    static void getHardwareVersion(char hardware_version[4]);
    static int64_t getTimestamp(void);
    static void getSoftwareVersion(char software_version[32]);
    static bool setBoardInfo(uint16_t board_type, uint32_t serial_num, char hardware_version[4], int64_t timestamp);

private:

    static uint8_t _calculate_eFuse_checksum(uint8_t* data);
    static bool _verify_eFuse_checksum(Module_eFuse_Info_t info);
    static uint16_t _type;

};