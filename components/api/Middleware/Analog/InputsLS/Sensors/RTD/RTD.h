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
        const std::array<Multiplexer_IO_t, 2>& muxIO) : 
            _adc(adc),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _adcInputs(adcInputs),
            _highSideMuxOutput(muxIO[0]),
            _lowSideMuxInput(muxIO[1]),
            _nbWires(adcInputs.size())
    {}

    float readRTD(void);
    float readTemperature(void);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    std::vector<ADC_Input_t> _adcInputs;
    Multiplexer_IO_t _highSideMuxOutput;
    Multiplexer_IO_t _lowSideMuxInput;
    int _nbWires;

    float _calculateRTD(const std::vector<uint16_t>& adcCodes);
    
};
