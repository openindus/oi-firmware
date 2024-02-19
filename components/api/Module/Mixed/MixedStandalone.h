/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedStandalone.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

#if defined(CONFIG_MIXED)

class MixedStandalone : public ModuleStandalone
{
public:

    static void init(void);

    /* DigitalOutput */
    static void digitalWrite(DigitalOutputNum_t doutNum, uint8_t level);
    static void digitalToggle(DigitalOutputNum_t doutNum);
    static void analogWrite(DigitalOutputNum_t doutNum, uint8_t duty);
    static float getCurrent(DigitalOutputNum_t doutNum);

    /* DigitalInput */
    static int digitalRead(DigitalInputNum_t dinNum);
    static void attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    static void detachInterrupt(DigitalInputNum_t dinNum);

    /* AnalogInput */
    static int analogRead(AnalogInputNum_t ain);
    static int analogReadMilliVolts(AnalogInputNum_t ain);
    static void analogReadMode(AnalogInputNum_t ain, AdcMode_t mode);
    static void analogReadResolution(AdcResBits_t res);
    static void analogReadReference(float ref);

    /* AnalogOutput */
    static void analogWriteVoltage(AnalogOutputNum_t aout, uint32_t value);
    static void analogWriteVoltageMilliVolts(AnalogOutputNum_t aout, uint32_t value);
    static void analogWriteVoltageMode(AnalogOutputNum_t aout, DacVoltageMode_t mode);
    static void analogWriteCurrent(AnalogOutputNum_t aout, uint32_t value);
    static void analogWriteCurrentMilliAmps(AnalogOutputNum_t aout, uint32_t value);
    static void analogWriteCurrentMode(AnalogOutputNum_t aout, DacCurrentMode_t mode);

private:

    static DigitalInput* din;
    static DigitalOutput* dout;

};

#endif