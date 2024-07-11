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
    TYPE_K,
};

struct TC_Coefficient_s {
    float Ti; // Initial temperature
    float Tf; // Final temperature
    float Ei; // Initial voltage
    float Ef; // Final voltage
    std::vector<float> d; // Coefficients
};

struct TC_Pinout_s {
    std::array<ADC_Input_t, 2> adcInputs;
};

class Thermocouple
{
public:

    Thermocouple(ADS114S0X* adc, const TC_Pinout_s& pins) : 
        _adc(adc), _adcInputs(pins.adcInputs), _type(TYPE_K) {}

    inline void setType(TC_Type_e type) {
        _type = type;
    }

    float readVoltage(void);
    float readTemperature(void);

private:

    ADS114S0X* _adc;
    std::array<ADC_Input_t, 2> _adcInputs;
    TC_Type_e _type;
    
    float _calculateTemperature(const std::vector<TC_Coefficient_s>& coefficients, float voltage);
};
