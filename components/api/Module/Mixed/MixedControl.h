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

    void digitalWrite(DigitalOutputNum_t stor, uint8_t level);
    int digitalRead(DigitalInputNum_t etor);
    void attachInterrupt(DigitalInputNum_t etor, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE);
    void detachInterrupt(DigitalInputNum_t etor);
    int analogRead(AnalogInputNum_t ana);
    int analogReadMilliVolts(AnalogInputNum_t ana);
    void analogReadMode(AnalogInputNum_t ana, AdcMode_t mode);
    void analogReadResolution(AdcResBits_t res);
    void analogReadReference(float ref);
    void analogWriteVoltage(AnalogOutputNum_t sana, uint32_t value);
    void analogWriteVoltageMilliVolts(AnalogOutputNum_t sana, uint32_t value);
    void analogWriteVoltageMode(AnalogOutputNum_t sana, DacVoltageMode_t mode);
    void analogWriteCurrent(AnalogOutputNum_t sana, uint32_t value);
    void analogWriteCurrentMilliAmps(AnalogOutputNum_t sana, uint32_t value);
    void analogWriteCurrentMode(AnalogOutputNum_t sana, DacCurrentMode_t mode);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif