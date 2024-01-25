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

#include "esp_err.h"
#include "esp_log.h"
#include "esp_console.h"

class UsbConsole
{
public:

    static void begin(void);
    static void end(void);

private:

    static esp_console_repl_t* _repl;
};
