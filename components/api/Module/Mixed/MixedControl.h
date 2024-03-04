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
#include "Mixed.h"

#if !defined(OI_MIXED)

class MixedControl : public ModuleControl, public Mixed
{
public:

    MixedControl(void) : ModuleControl() {}

    /* Digital Output */
    void digitalWrite(DigitalOutputNum_t num, uint8_t level);

    /* Digital Input */
    int digitalRead(DigitalInputNum_t num);
    void attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
        InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    void detachInterrupt(DigitalInputNum_t num);

    /* Analog Input */
    int analogRead(AnalogInputNum_t num);
    int analogReadMilliVolts(AnalogInputNum_t num);
    void analogReadMode(AnalogInputNum_t num, AdcMode_t mode);
    void analogReadResolution(AdcResBits_t res);
    void analogReadReference(float ref);

    /* Analog Output */
    void analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode);
    void analogWrite(AnalogOutput_Num_t num, float value);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif