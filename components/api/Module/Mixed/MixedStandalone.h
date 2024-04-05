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
#include "MixedInterface.h"
#include "MixedPinout.h"

#if defined(OI_MIXED)

class MixedStandalone : public ModuleStandalone, public MixedInterface
{
public:

    static int init(void);

    /* Digital Input */
    int digitalRead(DigitalInputNum_t num);
    void attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
        InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    void detachInterrupt(DigitalInputNum_t num);

    /* Digital Output */
    void digitalWrite(DigitalOutputNum_t num, uint8_t level);
    void digitalToggle(DigitalOutputNum_t num);
    void analogWrite(DigitalOutputNum_t num, uint8_t duty);
    float getCurrent(DigitalOutputNum_t num);

    /* Analog Input */
    void analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode);
    uint8_t analogInputGetMode(AnalogInput_Num_t num);
    void analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range);
    uint8_t analogInputGetVoltageRange(AnalogInput_Num_t num);
    int analogRead(AnalogInput_Num_t num);
    float analogReadVolt(AnalogInput_Num_t num);
    float analogReadMilliVolt(AnalogInput_Num_t num);
    float analogReadAmp(AnalogInput_Num_t num);
    float analogReadMilliAmp(AnalogInput_Num_t num);

    /* Analog Output */
    void analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode);
    void analogWrite(AnalogOutput_Num_t num, float value);

private:

    static DigitalInput* _din;
    static DigitalOutput* _dout;
    static AnalogInputs* _ains;
    static AnalogOutput* _aout[2];

};

#endif