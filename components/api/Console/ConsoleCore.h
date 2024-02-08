/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file ConsoleCore.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "CoreStandalone.h"

#if defined(CONFIG_CORE)

class ConsoleCore
{
public:

    static void registerCli(void);

private:

    static void _registerDigitalWrite(void);
    static void _registerDigitalRead(void);
    static void _registerAnalogRead(void);
    static void _registerAnalogReadMillivolts(void);
    static void _registerDigitalReadOverCurrent(void);

};

#endif
