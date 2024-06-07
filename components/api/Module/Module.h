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
#include "CLI_Led.h"
#include "CLI_Bus.h"
#include "CLI_Board.h"
#include "CLI_BusCtrl.h"

enum Module_Type_e {
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

class ModuleCLI:
    public CLI_Board,
    public CLI_Led,
    public CLI_Bus,
    public CLI_BusCtrl
{
public: 

    static inline int init(void) {
        int err = 0;
        
        err |= CLI_Board::init();
        err |= CLI_Led::init();
        err |= CLI_Bus::init();
        err |= CLI_BusCtrl::init();
        
        return err;
    }
};

class ModuleStandalone: public Board, public Led
{
public:

    static int init(uint16_t type);

    static inline void ledOn(LedColor_t color) {Led::on(color);};
    static inline void ledOff(void) {Led::off();};
    static inline void ledBlink(LedColor_t color, uint32_t period) {Led::blink(color, period);};

private:

    static uint16_t _type;

};
