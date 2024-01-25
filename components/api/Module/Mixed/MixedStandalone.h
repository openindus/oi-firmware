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
    static void digitalWrite(DigitalOutputNum_t stor, uint8_t level);

    /* DigitalInput */
    static int digitalRead(DigitalInputNum_t etorNum);
    static void attachInterrupt(DigitalInputNum_t etorNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    static void detachInterrupt(DigitalInputNum_t etorNum);

    /* AnalogInput */
    static int analogRead(AnalogInputNum_t ana);
    static int analogReadMilliVolts(AnalogInputNum_t ana);
    static void analogReadMode(AnalogInputNum_t ana, AdcMode_t mode);
    static void analogReadResolution(AdcResBits_t res);
    static void analogReadReference(float ref);

    /* AnalogOutput */
    static void analogWriteVoltage(AnalogOutputNum_t sana, uint32_t value);
    static void analogWriteVoltageMilliVolts(AnalogOutputNum_t sana, uint32_t value);
    static void analogWriteVoltageMode(AnalogOutputNum_t sana, DacVoltageMode_t mode);
    static void analogWriteCurrent(AnalogOutputNum_t sana, uint32_t value);
    static void analogWriteCurrentMilliAmps(AnalogOutputNum_t sana, uint32_t value);
    static void analogWriteCurrentMode(AnalogOutputNum_t sana, DacCurrentMode_t mode);

private:

    static DigitalInput* etor;

};

#endif