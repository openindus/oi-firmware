/**
 * @file StrainGauge.h
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"

struct StrainGauge_Pinout_s {
    ADC_Input_t sP; // Signal +
    ADC_Input_t sN; // Signal -
    ADC_Input_t eP; // Excitation +
    ADC_Input_t eN; // Excitation -
    Mux_IO_t hs; // Hight side (output)
    Mux_IO_t ls; // Low side (input)
};

class StrainGauge
{
public:

    StrainGauge(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux,
        const StrainGauge_Pinout_s& pins) : 
            _adc(adc),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _pins(pins)
    {}

    float read(void);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;

    StrainGauge_Pinout_s _pins;
    
};