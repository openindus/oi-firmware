/**
 * @file RawSensor.h
 * @brief RawSensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"

struct RawSensor_Pinout_s {
    std::array<ADC_Input_t, 2> adcInputs;
};

class RawSensor
{
public:

    RawSensor(ADS114S0X* adc, const RawSensor_Pinout_s& pins) : 
        _adc(adc),
        _adcInputs(pins.adcInputs),
        _gain(GAIN_1),
        _reference(REFERENCE_INTERNAL_2_5V) {}

    inline void setGain(Sensor_Gain_e gain) {
        _gain = gain;
    }

    inline void setReference(Sensor_Ref_e reference) {
        _reference = reference;
    }

    int16_t read(void);
    float readMillivolts(void);

private:

    ADS114S0X* _adc;
    std::array<ADC_Input_t, 2> _adcInputs;

    Sensor_Gain_e _gain;
    Sensor_Ref_e _reference;
};
