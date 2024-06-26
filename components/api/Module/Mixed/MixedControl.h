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
#include "MixedInterface.h"

#if !defined(OI_MIXED)

class MixedControl : public ModuleControl, public MixedInterface
{
public:

    MixedControl(int sn = 0) : 
        ModuleControl(sn),
        _isrCallback{NULL} {}

    /* Digital Output */
    void digitalWrite(DigitalOutputNum_t num, uint8_t level) override;

    /* Digital Input */
    int digitalRead(DigitalInputNum_t num) override;
    void attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
        InterruptMode_t mode=RISING_MODE, void* arg=NULL) override;
    void detachInterrupt(DigitalInputNum_t num) override;

    /* Analog Input */
    void analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode) override;
    void analogInputResolution(AnalogInput_Resolution_t res) override;
    void analogInputReference(float ref) override;
    int analogRead(AnalogInput_Num_t num) override;
    float analogReadMilliVolts(AnalogInput_Num_t num) override;
    float analogReadMilliAmps(AnalogInput_Num_t num);

    /* Analog Output */
    void analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode) override;
    void analogWrite(AnalogOutput_Num_t num, float value) override;

private:

    IsrCallback_t _isrCallback[4];
};

#endif