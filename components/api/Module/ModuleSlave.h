/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleSlave.h
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"
#include "Command.h"

class ModuleSlave
{
public:

    static void init(void);

    static void sendEvent(std::vector<uint8_t> msgBytes);

    inline static void addCtrlCallback(uint8_t ctrl, std::function<void(std::vector<uint8_t>&)> callback) {
        _ctrlCallbacks.insert({ctrl, callback});
    }

protected:

    static uint16_t _id;

private:

    static std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> _ctrlCallbacks;

    static void _busTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);
};
