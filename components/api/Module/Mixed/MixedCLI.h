/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "MixedStandalone.h"

#if defined(OI_MIXED)

class MixedCLI
{
public:

    static int init(void);

private:

    static void _registerDigitalWrite(void);
    static void _registerDigitalRead(void);
    static void _registerDigitalGetCurrent(void);
    static void _registerSetAnalogInputMode(void);
    static void _registerSetAnalogInputVoltageRange(void);
    static void _registerAnalogInputRead(void);
    static void _registerSetAnalogOutputMode(void);
    static void _registerAnalogOutputWrite(void);
};

#endif
