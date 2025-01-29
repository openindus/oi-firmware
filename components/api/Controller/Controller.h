/**
 * @file Controller.h
 * @brief Controller interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Controller.h"
#include "Cmd.h"
#include "Bus.h"
#include "Led.h"

typedef enum {
    STATE_UNDEFINED = (int) -1,
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
} Controller_State_t;

#if defined(MODULE_MASTER)

class Controller
{
public:

    Controller(uint16_t type, uint32_t sn);

    int sendCmd(const uint8_t cmd, std::vector<uint8_t>& msgBytes, bool ackNeeded = true);
    int sendRequest(std::vector<uint8_t>& msgBytes, bool ackNeeded = true);

    inline void setId(uint16_t id) { _id = id; }
    inline uint16_t getId(void) { return _id; }
    inline void setSN(uint32_t sn) { _sn = sn; }
    inline uint32_t getSN(void) { return _sn; }
    inline uint16_t getType(void) { return _type; }

    void restart(void);
    inline void ledOn(LedColor_t color) {_setLed(LED_ON, color, 0);};
    inline void ledOff(void) {_setLed(LED_OFF, LED_NONE, 0);};
    inline void ledBlink(LedColor_t color, uint32_t period) {_setLed(LED_BLINK, color, period);};

private:
    
    uint16_t _id; // Board id
    uint16_t _type; // Board 
    uint32_t _sn; // Serial number

    void _setLed(LedState_t state, LedColor_t color=LED_NONE, uint32_t period=0);

};

#endif