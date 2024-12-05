/**
 * @file Thermocouple.h
 * @brief Thermocouple
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"
#include "stds75.h"
#include "global_sensor.hpp"

#define TC_V_REF                    2.5
#define TC_GAIN                     8
#define TC_GAIN_REGISTER            GAIN_8
#define TC_ACQUISITION_REFERENCE    REFERENCE_INTERNAL_2_5V

struct TC_Coefficient_s {
    float i; // Initial temperature/voltage
    float f; // Final temperature/voltage
    std::vector<float> d; // Coefficients
};

struct TC_Pinout_s {
    std::array<ADC_Input_t, 2> adcInputs;
};

class Thermocouple: public Sensor
{
public:

    Thermocouple(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, Sensor_Pinout_s pinout, Sensor_Type_e type, uint32_t index) :
    Sensor(adc, highSideMux, lowSideMux, pinout, type, index)
    {
        _gain = TC_GAIN_REGISTER;
        _reference = TC_ACQUISITION_REFERENCE;
        _bias_active = true;
    }

    // - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
    float readMillivolts(bool print_result = false);
    float readTemperature(bool print_result = false);
    inline float read(bool print_result = false) { return readTemperature(print_result); }

protected:
    float _calculateVoltageFromTemperature(const std::vector<TC_Coefficient_s>& coefficients, float temperature, const std::vector<float> coeffs_A = {NAN, NAN, NAN});
    float _calculateTemperatureFromVoltage(const std::vector<TC_Coefficient_s>& coefficients, float voltage);
};
