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

class Thermocouple
{
public:

    Thermocouple(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux,
        const std::array<ADC_Input_t, 2>& adcInputs) : 
            _adc(adc),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _adcInputs(adcInputs)
    {}

    float readVoltage(uint32_t timeMs = 1000);
    float readTemperature(uint32_t timeMs = 1000);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    std::array<ADC_Input_t, 2> _adcInputs;
    
};
