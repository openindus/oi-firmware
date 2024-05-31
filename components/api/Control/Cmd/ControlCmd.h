/**
 * Copyright (C) OpenIndus, Inc - All Rights ReservedCLI
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and paramidential
 * 
 * @file ControlCmd.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Global.h"
#include "Control.h"
#include "Module.h"
#include "ControlMaster.h"
#include "Bus.h"

class ControlCmd
{
public:

    inline ControlCmd(uint16_t type, uint32_t sn) : _id(0xFFFF), _type(type), _sn(sn) {
        _instances.push_back(this);
    }

    void ledOn(LedColor_t color);
    void ledOff(void);
    void ledBlink(LedColor_t color, uint32_t period);

    friend class ControlMaster;

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
    static std::vector<ControlCmd*> _instances;

private:
    
    uint16_t _id; // Board id
    uint16_t _type; // Board 
    uint32_t _sn; // Serial number
    void _ledStatus(LedState_t state, LedColor_t color=LED_NONE, uint32_t period=0);

};

#endif