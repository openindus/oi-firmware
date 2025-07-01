/**
 * @file Slave.h
 * @brief Slave class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#pragma once

#include "Common.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "Types.h"
#include "FlashLoader.h"

class Slave
{
public:
    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static void sendEvent(std::vector<uint8_t> msgBytes);
    static void sendError(uint8_t errorCode);

    static inline void addCallback(uint8_t callbackId, std::function<void(std::vector<uint8_t> &)> callback) {
        _callbacks.insert({callbackId, callback});
    }

    static inline void addResetCallback(std::function<void(void)> callback) {
        _resetCallbacks.push_back(callback);
    }

    static inline void addEventCallback(uint8_t callbackId, std::function<void(std::vector<uint8_t> &)> callback) {
        _eventCallbacks.insert({callbackId, callback});
    }

protected:
    static uint16_t _id;

private:
    static State_e _state;
    static TaskHandle_t _busTaskHandle;

    static std::map<uint8_t, std::function<void(std::vector<uint8_t> &)>> _callbacks;
    static std::list<std::function<void(void)>> _resetCallbacks;
    static std::map<uint8_t, std::function<void(std::vector<uint8_t> &)>> _eventCallbacks;
    static std::vector<EventCallbackConfig_s> _eventCallbackConfigs;

    static void _busRsTask(void *pvParameters);
    static void _busCanTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);

    static int _registerCLI(void);
};

#endif