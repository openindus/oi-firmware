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

#if !defined(CONFIG_DISCRETE) && !defined(CONFIG_DISCRETE_VE)

class DiscreteControl : public ModuleControl
{
public:

    DiscreteControl(void) : ModuleControl() {}

    void digitalWrite(DigitalOutputNum_t stor, uint8_t level);
    void analogWrite(DigitalOutputNum_t stor, uint8_t duty);
    int digitalRead(DigitalInputNum_t etor);
    int analogRead(AnalogInputNum_t eana);
    void attachInterrupt(DigitalInputNum_t etor, IsrCallback_t callback, InterruptMode_t mode);
    void detachInterrupt(DigitalInputNum_t etor);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif