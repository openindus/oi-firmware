/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file UsbConsole.h
 * @brief OpenIndus USB Console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"
#include "driver/uart.h"

class UsbConsole
{
public:

    static void begin(void);
    static void end(void);
    static bool checkUserActivation(uint32_t timeoutMs);

private:

    static void _listenTask(void *pvParameters);
    static esp_console_repl_t* _repl;
    static bool keepFlag;
    static bool continueListenTask;
    static TaskHandle_t listenTaskHandle;
};
