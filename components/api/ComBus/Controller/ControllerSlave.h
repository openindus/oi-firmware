/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControllerSlave.h
 * @brief Functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Controller.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "FlashLoader.h"
#include "Protocol.h"

#if defined(MODULE_SLAVE)

class ControllerSlave
{
public:

    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static void sendEvent(std::vector<uint8_t> msgBytes);

    inline static void addCtrlCallback(uint8_t ctrl, std::function<void(std::vector<uint8_t>&)> callback) {
        _ctrlCallbacks.insert({ctrl, callback});
    }

protected:

    static uint16_t _id;

private:

    static Controller_State_t _state;
    static TaskHandle_t _taskHandle;

    static std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> _ctrlCallbacks;

    static void _busTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);
};

#endif