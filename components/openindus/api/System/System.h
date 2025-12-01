/**
 * @file System.h
 * @brief System
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Types.h"

/**
 * @class System
 */
class System
{
public:
    /**
     * @brief Initialize the system
     * @return int Status code (0 = success, non-zero = error)
     */
    static int init(void);

    /**
     * @brief Start the system
     */
    static void start(void);

    /**
     * @brief Stop the system
     */
    static void stop(void);

    /**
     * @brief Handle system errors
     * @param errorCode Error code
     */
    __attribute__((weak)) static void handleError(int errorCode);

private:
    static void _createMainTask(void);
    static TaskHandle_t _mainTaskHandle;
};