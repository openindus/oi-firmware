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
        const std::vector<ADC_Input_t>& adcInputs,
        const Multiplexer_IO_t& highSideMuxOutput, 
        const Multiplexer_IO_t& lowSideMuxInput) : 
            _adc(adc),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _adcInputs(adcInputs),
            _highSideMuxOutput(highSideMuxOutput),
            _lowSideMuxInput(lowSideMuxInput),
            _nbWires(adcInputs.size())
    {}

    float readRTD(uint32_t timeout_ms = 1000);
    float readTemperature(uint32_t timeout_ms = 1000);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    std::vector<ADC_Input_t> _adcInputs;
    Multiplexer_IO_t _highSideMuxOutput;
    Multiplexer_IO_t _lowSideMuxInput;
    int _nbWires;
    
};
