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

    static MixedStandalone* _mixed;

    static int _digitalWrite(int argc, char **argv);
    static void _registerDigitalWrite(void);

    static int _digitalRead(int argc, char **argv);
    static void _registerDigitalRead(void);

    static int _getDigitalCurrent(int argc, char **argv);
    static void _registerGetDigitalCurrent(void);

    static int _setAnalogVoltageRange(int argc, char **argv);
    static void _registerSetAnalogVoltageRange(void);

    static int _analogReadVoltage(int argc, char **argv);
    static void _registerAnalogReadVoltage(void);
};

#endif
