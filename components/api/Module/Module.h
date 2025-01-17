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
#include "Board.h"
#include "Led.h"
#include "Bus.h"

class Module: 
    private Board, 
    private Led,
    private Bus
{
protected:

    static int init(uint16_t type);

public:

    static inline void restart(void) {Board::restart();};

    static inline void ledOn(LedColor_t color) {Led::on(color);};
    static inline void ledOff(void) {Led::off();};
    static inline void ledBlink(LedColor_t color, uint32_t period) {Led::blink(color, period);};

#ifndef LINUX_ARM
#if defined(MODULE_MASTER) || defined(MODULE_SLAVE)

    static inline void busPowerOn(void) { BusIO::powerOn(); }
    static inline void busPowerOff(void) { BusIO::powerOff(); }

#endif
#endif

private:

    static uint16_t _type;

};