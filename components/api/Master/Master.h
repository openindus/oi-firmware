/**
 * @file Master.h
 * @brief Master class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Common.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "Types.h"
#include "Controller.h"

class Master
{
public:

    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static bool autoId(void);
    static void program(uint16_t type, uint32_t sn);

    /* Cmd */
    static void restart(const uint16_t id);
    static bool ping(uint16_t type, uint32_t sn);
    static void setLed(const uint16_t id, const uint8_t state, const uint8_t color, const uint32_t period);
    static std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> discoverSlaves(void);
    static void getBoardInfo(uint16_t type, uint32_t sn, Board_Info_t* board_info);

    template <typename... Args> 
    static int performRequest(const uint16_t id, const uint8_t request, Args... args);
    static int performRequest(const uint16_t id, const uint8_t request, std::vector<uint8_t> &msgBytes, bool ackNeeded = true);
    static int performRequest(const uint16_t id, std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    static void resetModules(void);

    static inline void addControllerInstance(Controller* controller) {
        _controllerInstances.push_back(controller);
    }

    static inline void addEventProcessCallback(uint8_t event, uint16_t id, std::function<void(uint8_t*)>callback) {
        _eventProcessCallbacks.insert({std::make_pair(event, id), callback});
    }

    static inline void removeEventProcessCallback(uint8_t event, uint16_t id) {
        _eventProcessCallbacks.erase(std::make_pair(event, id));
    }

    static void addErrorCallback(std::function<void(int)> callback) {
        _errorCallback = callback;
    }

    enum State_e{
        STATE_UNDEFINED = (int)-1,
        STATE_IDLE      = (int)0,
        STATE_RUNNING   = (int)1,
        STATE_ERROR     = (int)2
    };

private:

    static State_e _state;

#ifndef LINUX_ARM
    static TaskHandle_t _taskHandle;
#endif

    // List of ids and serial number received via CMD_DISCOVER < ID < TYPE, SN > >
    static std::map<uint16_t, std::pair<uint16_t, uint32_t>, std::greater<uint16_t>> _ids;

    static uint16_t _getIdFromSerialNumAndType(uint16_t type, uint32_t sn);

    /** @todo _processEvent() */

    static void _busCanTask(void *pvParameters);
    static void _programmingTask(void *pvParameters);
    static SemaphoreHandle_t _requestMutex;

    static std::map<std::pair<uint8_t,uint16_t>, std::function<void(uint8_t*)>> _eventProcessCallbacks;
    static std::vector<Controller*> _controllerInstances;
    static std::function<void(int)> _errorCallback;

    static int _registerCLI(void);

};

#endif