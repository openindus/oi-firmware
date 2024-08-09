/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControllerMaster.h
 * @brief Functions for module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Global.h"
#include "Controller.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "UsbConsole.h"
#include "UsbSerial.h"
#include "Protocol.h"
#include "Controller.h"

class ControllerMaster
{
public:

    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static bool autoId(void);
    static void autoTest(void);
    static void program(uint16_t type, uint32_t sn);
    static bool ping(uint16_t type, uint32_t sn); 

    static void getBoardInfo(uint16_t type, uint32_t sn, Board_Info_t* board_info);
    static std::map<uint16_t,std::pair<uint16_t, uint32_t>,std::greater<uint16_t>> discoverSlaves(void);

    static inline void addControllerInstance(Controller* controller) {
        _instances.push_back(controller);
    }

    static inline void addEventCallback(uint8_t event, uint16_t id, std::function<void(uint8_t)>callback) {
        _eventCallbacks.insert({std::make_pair(event, id), callback});
    }

    static inline void removeEventCallback(uint8_t event, uint16_t id) {
        _eventCallbacks.erase(std::make_pair(event, id));
    }

private:

    static Controller_State_t _state;
    static TaskHandle_t _taskHandle;

    // List of ids and serial number received via CMD_DISCOVER < ID < TYPE, SN > >
    static std::map<uint16_t, std::pair<uint16_t, uint32_t>, std::greater<uint16_t>> _ids;

    static uint16_t _getIdFromSerialNumAndType(uint16_t type, uint32_t sn);
    static void _busTask(void *pvParameters);
    static void _programmingTask(void *pvParameters);

    static std::map<std::pair<uint8_t,uint16_t>, std::function<void(uint8_t)>> _eventCallbacks;
    static std::vector<Controller*> _instances;

};

#endif