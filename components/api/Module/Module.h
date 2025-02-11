/**
 * @file Module.h
 * @brief Module interface
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Board.h"
#include "Bus.h"
#include "Global.h"
#include "Led.h"
#include "Module.h"
#include "ModulePinout.h"

class Module : private Board, private Led, private Bus
{
  protected:
    static int init(uint16_t type);

  public:
    static inline void restart(void) { Board::restart(); };
    static inline float getTemperature(void) { return Board::getTemperature(); };

    static inline void ledOn(LedColor_t color) { Led::on(color); };
    static inline void ledOff(void) { Led::off(); };
    static inline void ledBlink(LedColor_t color, uint32_t period) { Led::blink(color, period); };

#ifndef LINUX_ARM
#if defined(MODULE_MASTER) || defined(MODULE_SLAVE)

    static inline void busPowerOn(void) { BusIO::powerOn(); }
    static inline void busPowerOff(void) { BusIO::powerOff(); }

#endif
#endif

  private:
    static uint16_t _type;
};