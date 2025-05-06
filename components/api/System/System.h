/**
 * @file System.h
 * @brief System main
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

class System
{
public:
    static int init(void);
    static void start(void);
    static void stop(void);

    __attribute__((weak)) static void handleError(int errorCode);

private:
    static void _mainTask(void *pvParameters);
    static TaskHandle_t _mainTaskHandle;
};