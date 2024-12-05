/**
 * @file RawSensor.h
 * @brief RawSensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "global_sensor.hpp"

class RawSensor: public Sensor
{
public:

    RawSensor(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, const Sensor_Pinout_s& pins, uint32_t index) :
    Sensor(adc, highSideMux, lowSideMux, pins, RAW_SENSOR, index) {}

    // - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
    float readMillivolts(bool print_result = false);
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);

    protected:
    void setMuxParameter(Sensor_Parameter_e parameter, Mux_Parameter_u value);
    inline void setGain(Sensor_Gain_e gain) { _gain = gain; }
    inline void setReference(Sensor_Ref_e reference) { _reference = reference; }
    inline void setBiasActive(bool active) { _bias_active = active; }
    inline void setExcitation(Sensor_Excitation_e excitation) { _excitation = excitation; }
};
