/**
 * @file Controller.h
 * @brief Controller interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

typedef enum {
    STATE_UNDEFINED = (int) -1,
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
} Controller_State_t;

#if defined(MODULE_MASTER)

#include "Global.h"
#include "Controller.h"
#include "Module.h"
#include "ControllerMaster.h"
#include "Bus.h"

class Controller
{
public:

    Controller(uint16_t type, uint32_t sn) : _id(0xFFFF), _type(type), _sn(sn) {
        _instances.push_back(this);
    }

    inline void ledOn(LedColor_t color) {_ledStatus(LED_ON, color, 0);};
    inline void ledOff(void) {_ledStatus(LED_OFF, LED_NONE, 0);};
    inline void ledBlink(LedColor_t color, uint32_t period) {_ledStatus(LED_BLINK, color, period);};

    friend class ControllerMaster;

    int request(std::vector<uint8_t>& msgBytes, bool ackNeeded = true);

    inline void setId(uint16_t id) { _id = id; }

    inline uint16_t getId(void) { return _id; }

    inline void setSN(uint32_t sn) { _sn = sn; }

    inline uint32_t getSN(void) { return _sn; }

    inline uint16_t getType(void) { return _type; }

    inline static void addEventCallback(uint8_t event, uint16_t id, std::function<void(uint8_t)>callback) {
        _eventCallbacks.insert({std::make_pair(event, id), callback});
    }

    inline static void removeEventCallback(uint8_t event, uint16_t id) {
        _eventCallbacks.erase(std::make_pair(event, id));
    }

protected:

    static std::map<std::pair<uint8_t,uint16_t>, std::function<void(uint8_t)>> _eventCallbacks;
    static std::vector<Controller*> _instances;

private:
    
    uint16_t _id; // Board id
    uint16_t _type; // Board 
    uint32_t _sn; // Serial number
    void _ledStatus(LedState_t state, LedColor_t color=LED_NONE, uint32_t period=0);

};

#endif