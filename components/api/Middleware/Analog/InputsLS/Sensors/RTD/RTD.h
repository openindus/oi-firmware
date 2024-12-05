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
#include "RawSensor.h"

enum RTD_Type_e {
    PT100 = 0,
    PT1000,
};

#define RTD_R_REF                       3000
#define RTD_PT100_GAIN                  8
#define RTD_PT1000_GAIN                 1
#define RTD_PT100_GAIN_REGISTER         GAIN_8
#define RTD_PT1000_GAIN_REGISTER        GAIN_1
#define RTD_PT100_EXCITATION_CURRENT    EXCITATION_750_UA
#define RTD_PT1000_EXCITATION_CURRENT   EXCITATION_250_UA
#define RTD_ACQUISITION_REFERENCE       REFERENCE_IDAC_1

class RTD: public Sensor
{
public:

    RTD(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, Sensor_Pinout_s pinout, RTD_Type_e type, uint32_t index) :
    Sensor(adc, highSideMux, lowSideMux, pinout, type == PT100 ? RTD_PT100 : RTD_PT1000, index)
    {
        _gain = type == PT100 ? RTD_PT100_GAIN_REGISTER : RTD_PT1000_GAIN_REGISTER;
        _bias_active = false;
        _mux_config.input = INPUT_IDAC1;
        _mux_config.output = OUTPUT_RBIAS_RTD;
        _reference = RTD_ACQUISITION_REFERENCE;
        _excitation = type == PT100 ? RTD_PT100_EXCITATION_CURRENT : RTD_PT1000_EXCITATION_CURRENT;
    }

    // - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
    float readResistor(bool print_result = false);
    float readTemperature(bool print_result = false);
    inline float read(bool print_result = false) { return readResistor(print_result); }

private:
    float _calculateRTD(int16_t adcCode);
};
