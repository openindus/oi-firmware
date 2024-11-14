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


struct TC_Coefficient_s {
    float i; // Initial temperature/voltage
    float f; // Final temperature/voltage
    std::vector<float> d; // Coefficients
};

struct TC_Pinout_s {
    std::array<ADC_Input_t, 2> adcInputs;
};

class Thermocouple
{
public:

    Thermocouple(ADS114S0X* adc, const TC_Pinout_s& pins, Sensor_Type_e type) : 
        _adc(adc),
        _adcInputs(pins.adcInputs),
        _type(type) {}

    float readMillivolts(void);
    float readTemperature(void);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;

    std::array<ADC_Input_t, 2> _adcInputs;
    Sensor_Type_e _type;

    float _calculateVoltageFromTemperature(const std::vector<TC_Coefficient_s>& coefficients, const std::vector<float> coeffs_A, float temperature);

    float _calculateTemperatureFromVoltage(const std::vector<TC_Coefficient_s>& coefficients, float voltage);
};
