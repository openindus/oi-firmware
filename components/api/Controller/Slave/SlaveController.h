/**
 * @file SlaveController.h
 * @brief Slave Controller class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Controller.h"
#include "Board.h"
#include "Led.h"
#include "Bus.h"
#include "FlashLoader.h"

#if defined(MODULE_SLAVE)

class SlaveController
{
public:

    static int init(void);

    static void start(void);
    static void stop(void);
    static int getStatus(void);

    static void sendEvent(std::vector<uint8_t> msgBytes);

    inline static void addCmdHandler(uint8_t cmd, std::function<void(std::vector<uint8_t>&)> callback) {
        _handlers.insert({cmd, callback});
    }

protected:

    static uint16_t _id;

private:

    static Controller_State_t _state;
    static TaskHandle_t _taskHandle;

    static std::map<uint8_t, std::function<void(std::vector<uint8_t>&)>> _handlers;

    static void _busTask(void *pvParameters);
    static void _heartbeatTask(void *pvParameters);

    static int _registerCLI(void);
};

#endif