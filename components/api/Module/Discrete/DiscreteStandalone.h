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

class DiscreteStandalone : public ModuleStandalone
{
public:

    static void init(void);

    /* DigitalOutput */
    static void digitalWrite(DigitalOutputNum_t doutNum, uint8_t level);
    static void digitalToggle(DigitalOutputNum_t doutNum);
    static void analogWrite(DigitalOutputNum_t doutNum, uint8_t duty);
    static float getCurrent(DigitalOutputNum_t doutNum);

    /* DigitalInput */
    static int digitalRead(DigitalInputNum_t dinNum);
    static void attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    static void detachInterrupt(DigitalInputNum_t dinNum);

    /* AnalogInput */
    static int analogRead(AnalogInput_Num_t eana);
    static int analogReadMilliVolts(AnalogInput_Num_t eana);

private:

    static DigitalInput* din;
    static DigitalOutput* dout;
    static esp_adc_cal_characteristics_t _adc1Characteristics;

};

#endif