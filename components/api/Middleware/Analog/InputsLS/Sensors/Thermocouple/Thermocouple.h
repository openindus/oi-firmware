/**
 * @file Thermocouple.h
 * @brief Thermocouple
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"

enum TC_Type_e {
    TYPE_K = (int) 0,
};

struct TC_Coefficient_s {
    float Ti; // Initial temperature
    float Tf; // Final temperature
    float Ei; // Initial voltage
    float Ef; // Final voltage
    std::vector<float> d; // Coefficients
};

class Thermocouple
{
public:

    Thermocouple(TC_Type_e type, ADS114S0X* adc, const std::array<ADC_Input_t, 2>& adcInputs) : 
        _type(type), _adc(adc), _adcInputs(adcInputs) {}

    float readVoltage(void);
    float readTemperature(void);

private:

    TC_Type_e _type;

    ADS114S0X* _adc;
    std::array<ADC_Input_t, 2> _adcInputs;
    
    float _calculateTemperature(const std::vector<TC_Coefficient_s>& coefficients, float voltage);
};
