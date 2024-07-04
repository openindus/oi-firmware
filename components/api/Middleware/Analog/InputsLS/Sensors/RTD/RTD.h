/**
 * @file RTD.h
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ADS114S0X.h"
#include "Multiplexer.h"

class RTD
{
public:

    RTD(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux,
        ADC_Input_t adcInputP, ADC_Input_t adcInputN,
        Multiplexer_IO_t highSideMuxOutput, Multiplexer_IO_t lowSideMuxInput) : 
            _adc(adc),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _adcInputP(adcInputP),
            _adcInputN(adcInputN),
            _highSideMuxOutput(highSideMuxOutput),
            _lowSideMuxInput(lowSideMuxInput)
    {}

    int select(void);

    float readRTD(uint32_t timeout_ms);
    float readTemperature(uint32_t timeout_ms);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    ADC_Input_t _adcInputP;
    ADC_Input_t _adcInputN;
    Multiplexer_IO_t _highSideMuxOutput;
    Multiplexer_IO_t _lowSideMuxInput;
    
};
