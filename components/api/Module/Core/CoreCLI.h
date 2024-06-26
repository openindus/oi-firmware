/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file CoreCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "CoreStandalone.h"

#if defined(OI_CORE)

class CoreCLI
{
public:

    static void init(void);

private:

    static void _registerDigitalWrite(void);
    static void _registerDigitalRead(void);
    static void _registerAnalogRead(void);
    static void _registerAnalogReadMillivolts(void);
    static void _registerGetCurrentLevel(void);
    static void _registerDate(void);

};

#endif
