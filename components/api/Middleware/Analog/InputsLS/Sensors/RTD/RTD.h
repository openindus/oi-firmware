/**
 * @file RTD.h
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Sensor.h"

enum RTD_Type_e {
    PT100 = 0,
    PT1000,
};

struct RTD_Pinout_s {
    std::vector<ADC_Input_t> adcInputs; // 2 or 3 inputs
    Mux_IO_t hsMuxOutput;
    Mux_IO_t lsMuxInput;
};

class RTD
{
public:

    RTD(ADS114S0X* adc, Sensor_Type_e type, Multiplexer* highSideMux, Multiplexer* lowSideMux, const RTD_Pinout_s& pins) : 
            _adc(adc),
            _type(type),
            _highSideMux(highSideMux),
            _lowSideMux(lowSideMux),
            _adcInputs(pins.adcInputs),
            _hsMuxOutput(pins.hsMuxOutput),
            _lsMuxInput(pins.lsMuxInput) {}

    float readRTD(void);
    float readTemperature(void);

private:

    ADS114S0X* _adc;
    Sensor_Type_e _type;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    std::vector<ADC_Input_t> _adcInputs;
    Mux_IO_t _hsMuxOutput;
    Mux_IO_t _lsMuxInput;

    float _calculateRTD(int16_t adcCode);

};
