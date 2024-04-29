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

    static int _digitalWrite(int argc, char **argv);
    static void _registerDigitalWrite(void);

    static int _digitalRead(int argc, char **argv);
    static void _registerDigitalRead(void);

    static int _digitalGetCurrent(int argc, char **argv);
    static void _registerDigitalGetCurrent(void);

    static int _setAnalogInputMode(int argc, char **argv);
    static void _registerSetAnalogInputMode(void);

    static int _setAnalogInputVoltageRange(int argc, char **argv);
    static void _registerSetAnalogInputVoltageRange(void);

    static int _analogInputRead(int argc, char **argv);
    static void _registerAnalogInputRead(void);

    static int _setAnalogOutputMode(int argc, char **argv);
    static void _registerSetAnalogOutputMode(void);

    static int _analogOutputWrite(int argc, char **argv);
    static void _registerAnalogOutputWrite(void);
};

#endif
