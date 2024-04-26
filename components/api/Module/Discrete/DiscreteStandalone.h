/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteStandalone.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

class DiscreteStandalone : public ModuleStandalone, public AnalogInputs
{
public:

    static void init(void);

    /* DigitalOutputs */
    static void digitalWrite(DigitalOutputNum_t doutNum, uint8_t level);
    static void digitalToggle(DigitalOutputNum_t doutNum);
    static void digitalPWM(DigitalOutputNum_t doutNum, uint8_t duty);
    static float digitalGetCurrent(DigitalOutputNum_t doutNum);

    /* DigitalInputs */
    static int digitalRead(DigitalInputNum_t dinNum);
    static void attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    static void detachInterrupt(DigitalInputNum_t dinNum);

    /* AnalogInputs */
    // static int analogRead(AnalogInput_Num_t ainNum);
    // static float analogReadVolt(AnalogInput_Num_t ainNum);
    // static float analogReadMilliVolt(AnalogInput_Num_t ainNum);
    // static int setAnalogCoeffs(float* coeff_a, float* coeff_b);

private:

    static DigitalInputs* din;
    static DigitalOutputs* dout;

};

#endif