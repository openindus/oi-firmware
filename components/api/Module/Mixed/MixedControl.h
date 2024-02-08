/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedControl.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"

#if !defined(CONFIG_MIXED)

class MixedControl : public ModuleControl
{
public:

    MixedControl(void) : ModuleControl() {}

    void digitalWrite(DigitalOutputNum_t dout, uint8_t level);
    int digitalRead(DigitalInputNum_t din);
    void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE);
    void detachInterrupt(DigitalInputNum_t din);
    int analogRead(AnalogInputNum_t ain);
    int analogReadMilliVolts(AnalogInputNum_t ain);
    void analogReadMode(AnalogInputNum_t ain, AdcMode_t mode);
    void analogReadResolution(AdcResBits_t res);
    void analogReadReference(float ref);
    void analogWriteVoltage(AnalogOutputNum_t aout, uint32_t value);
    void analogWriteVoltageMilliVolts(AnalogOutputNum_t aout, uint32_t value);
    void analogWriteVoltageMode(AnalogOutputNum_t aout, DacVoltageMode_t mode);
    void analogWriteCurrent(AnalogOutputNum_t aout, uint32_t value);
    void analogWriteCurrentMilliAmps(AnalogOutputNum_t aout, uint32_t value);
    void analogWriteCurrentMode(AnalogOutputNum_t aout, DacCurrentMode_t mode);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif