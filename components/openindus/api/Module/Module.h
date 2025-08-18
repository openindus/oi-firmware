/**
 * @file Module.h
 * @brief Module interface
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Board.h"
#include "Bus.h"
#include "Led.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

class Module : private Board, private Led, private Bus
{
public:
    static inline void restart(void) { Board::restart(); };
    static inline float getTemperature(void) { return Board::getTemperature(); };
    static inline bool checkBootError(void) { return Board::checkBootError(); };

    static inline void ledOn(LedColor_t color) { Led::on(color); };
    static inline void ledOff(void) { Led::off(); };
    static inline void ledBlink(LedColor_t color, uint32_t period) { Led::blink(color, period); };

#if defined(CONFIG_MODULE_MASTER) || defined(CONFIG_MODULE_SLAVE)
    static inline void busPowerOn(void) { BusIO::powerOn(); }
    static inline void busPowerOff(void) { BusIO::powerOff(); }
#endif

protected:
    static int init(uint16_t type);
    
    /* ADC handles */
    static adc_oneshot_unit_handle_t adcHandles[2];

private:
    static uint16_t _type;

    static int _initBoardInfos(void);
    static void _initADC(void);
};