/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleDiscrete.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "DiscreteStandalone.h"

#if defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)

class ConsoleDiscrete
{
public:

    static void registerCli(void);

private:

    static void _registerDigitalWrite(void);
    static void _registerDigitalRead(void);
    static void _registerGetCurrent(void);
    static void _registerAnalogRead(void);

};

#endif
