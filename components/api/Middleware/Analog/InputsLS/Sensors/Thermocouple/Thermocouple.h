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

typedef enum {
    TYPE_R = 0,
    TYPE_S,
    TYPE_K,
    TYPE_E,
    TYPE_J,
    TYPE_T,
    TYPE_B,
    TYPE_N
} TC_Type_t;

class Thermocouple
{
public:

    Thermocouple(ADS114S0X* adc, const std::array<ADC_Input_t, 2>& adcInputs) : 
        _adc(adc), _adcInputs(adcInputs) {}

    float readVoltage(void);
    float readTemperature(void);

private:

    ADS114S0X* _adc;
    std::array<ADC_Input_t, 2> _adcInputs;
    
    float _calculateTemperature(const std::vector<float>& coefficients, float voltage);
};
