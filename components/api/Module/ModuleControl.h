/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and paramidential
 * 
 * @file ModuleControl.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Global.h"
#include "ModuleStandalone.h"
#include "Command.h"
#include "ModuleMaster.h"

class ModuleControl
{
public:

    inline ModuleControl(int sn) : _id(0xFFFF), _sn(sn) {
        _instances.push_back(this);
    }

    void ledOn(LedColor_t color);
    void ledOff(void);
    void ledBlink(LedColor_t color, uint32_t period);

    // friend class ModuleMaster;

    int ctrlRequest(std::vector<uint8_t>& msgBytes);

    inline static void setId(ModuleControl* instance, uint16_t id) {
        instance->_id = id;
    }

    inline static uint16_t getId(ModuleControl* instance) {
        return instance->_id;
    }

    inline static void setSN(ModuleControl* instance, int sn) {
        instance->_sn = sn;
    }

    inline static int getSN(ModuleControl* instance) {
        return instance->_sn;
    }

    inline static void addEventCallback(ModuleControl* instance, uint8_t event, std::function<void(uint8_t)>callback) {
        _eventCallbacks.insert({std::make_pair(event, instance->_id), callback});
    }

    inline static std::vector<ModuleControl*> getAllInstances(void) {
        return _instances;
    }

    inline static std::map<std::pair<uint8_t,uint16_t>, std::function<void(uint8_t)>> getEventCallbacks(void) {
        return _eventCallbacks;
    }

protected:

    uint16_t _id; // Board id
    int _sn; // Serial number

private:

    static std::vector<ModuleControl*> _instances;
    static std::map<std::pair<uint8_t,uint16_t>, std::function<void(uint8_t)>> _eventCallbacks;

    void _ledStatus(LedState_t state, LedColor_t color=LED_NONE, uint32_t period=0);

};

#endif