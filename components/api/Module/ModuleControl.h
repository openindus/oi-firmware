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

#include "Global.h"
#include "ModuleStandalone.h"
#include "ModuleCmd.h"
#include "ModuleMaster.h"

class ModuleControl
{
public:

    inline ModuleControl(int sn) {
        _instances.push_back(this);
        _sn = sn;
    }

    friend class ModuleMaster;

    uint32_t request(ModuleCmd_RequestMsg_t msg);

    void ledOn(LedColor_t color);
    void ledOff(void);
    void ledBlink(LedColor_t color, uint32_t period);

protected:

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

    static std::vector<ModuleControl*> _instances;

    uint16_t _id;
    int _sn;

private:

    void _ledCtrl(LedState_t state, LedColor_t color=LED_NONE, uint32_t period=0);

};