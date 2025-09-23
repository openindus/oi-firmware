/**
 * @file Master.h
 * @brief Master class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

#if defined(CONFIG_MODULE_MASTER)

#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "Types.h"
#include "ModuleControl.h"

class Master
{
public:
    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static bool autoId(void);
    static void program(uint16_t boardType, uint32_t boardSN);

    static void moduleRestart(const uint16_t slaveId);
    static bool ping(uint16_t boardType, uint32_t boardSN);
    static void ledCtrl(const uint16_t slaveId, const uint8_t state, const uint8_t color = LED_NONE, const uint32_t period = 0);

    using SlaveInfo = std::pair<uint16_t, uint32_t>; // Slave ID and serial number
    static std::map<uint16_t, SlaveInfo, std::greater<uint16_t>> discoverSlaves(void);
    static void getBoardInfo(uint16_t boardType, uint32_t boardSN, Board_Info_t* info);

    static int runCallback(const uint16_t slaveId, const uint8_t callbackId, std::vector<uint8_t> &args, bool ackNeeded = true);
    static int runCallback(const uint16_t slaveId, std::vector<uint8_t> &msgBytes, bool ackNeeded = true);

    static void resetModules(void);

    static void registerEventCallback(uint16_t slaveId, ModuleControl* module, 
        uint8_t eventId, uint8_t eventArg, 
        uint8_t callbackId, std::vector<uint8_t> callbackArgs);

    static uint16_t getSlaveId(uint16_t boardType, uint32_t boardSN);

    static inline void addModuleControlInstance(ModuleControl* module) {
        _modules.push_back(module);
    }

    static inline void addEventCallback(uint8_t eventId, uint16_t slaveId, std::function<void(uint8_t*)>callback) {
        _eventCallbacks.insert({std::make_pair(eventId, slaveId), callback});
    }

    static inline void removeEventCallback(uint8_t eventId, uint16_t slaveId) {
        _eventCallbacks.erase(std::make_pair(eventId, slaveId));
    }

    static void addErrorCallback(std::function<void(int)> callback) {
        _errorCallback = callback;
    }

private:
    static State_e _state;
    static TaskHandle_t _busTaskHandle;
    static TaskHandle_t _ledSyncTaskHandle;
    static SemaphoreHandle_t _callbackMutex;

    static void _busCanTask(void *pvParameters);
    static void _programmingTask(void *pvParameters);
    static void _ledSyncTask(void *pvParameters);

    using EventKey = std::pair<uint8_t, uint16_t>; // Event ID and Module ID
    using EventCallback = std::function<void(uint8_t*)>;

    static std::vector<ModuleControl*> _modules;
    static std::map<uint16_t, SlaveInfo, std::greater<uint16_t>> _slaveInfos;
    static std::map<EventKey, EventCallback> _eventCallbacks;
    static std::function<void(int)> _errorCallback;

    static int _registerCLI(void);
};

#endif