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
    /** @todo heartbeat(); */
    static void sendError(uint8_t errorCode);

    static inline void addRequestCallback(uint8_t request, std::function<void(std::vector<uint8_t> &)> callback) {
        _requestCallbacks.insert({request, callback});
    }

    static inline void addResetFunction(std::function<void(void)> resetFunction) {
        _resetFunctions.push_back(resetFunction);
    }

    enum State_e {
        STATE_UNDEFINED = (int)-1,
        STATE_IDLE      = (int)0,
        STATE_RUNNING   = (int)1,
        STATE_ERROR     = (int)2
    };

protected:
    static uint16_t _id;

private:
    static State_e _state;
    static TaskHandle_t _taskHandle;

    static std::map<uint8_t, std::function<void(std::vector<uint8_t> &)>> _requestCallbacks;
    static std::list<std::function<void(void)>> _resetFunctions;

    /** @todo _processRequest() */

    static void _busRsTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);

    static int _registerCLI(void);
};

#endif