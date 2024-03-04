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
#include "Mixed.h"

#if defined(OI_MIXED)

class MixedStandalone : public ModuleStandalone, public Mixed
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
    int analogRead(AnalogInputNum_t num);
    int analogReadMilliVolts(AnalogInputNum_t num);
    void analogReadMode(AnalogInputNum_t num, AdcMode_t mode);
    void analogReadResolution(AdcResBits_t res);
    void analogReadReference(float ref);

    /* Analog Output */
    void analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode);
    void analogWrite(AnalogOutput_Num_t num, float value);

private:

    static DigitalInput* _din;
    static DigitalOutput* _dout;
    // static AnalogInput* _ain;
    static AnalogOutput* _aout[2];

};

#endif