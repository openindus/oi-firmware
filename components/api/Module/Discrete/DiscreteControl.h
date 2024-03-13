/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"

#if !defined(OI_DISCRETE) && !defined(OI_DISCRETE_VE)

class DiscreteControl : public ModuleControl
{
public:

    DiscreteControl(int sn = 0) : ModuleControl(sn) {}

    void digitalWrite(DigitalOutputNum_t dout, uint8_t level);
    void analogWrite(DigitalOutputNum_t dout, uint8_t duty);
    int digitalRead(DigitalInputNum_t din);
    int analogRead(AnalogInput_Num_t eana);
    void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode);
    void detachInterrupt(DigitalInputNum_t din);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif