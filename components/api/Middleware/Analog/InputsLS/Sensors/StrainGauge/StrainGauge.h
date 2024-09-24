/**
 * @file StrainGauge.h
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "_ADS114S0X.h"
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

enum StrainGauge_Excitation_e {
    EXCITATION_VOLTAGE_3V3 = 0,
    EXCITATION_VOLTAGE_5V,
    EXCITATION_VOLTAGE_USER,
    EXCITATION_CURRENT_10UA,
    EXCITATION_CURRENT_100UA,
    EXCITATION_CURRENT_500UA,
    EXCITATION_CURRENT_1000UA,
    EXCITATION_CURRENT_2000UA
};

class StrainGauge
{
public:

    StrainGauge(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, const StrainGauge_Pinout_s& pins) : 
                _adc(adc),
                _highSideMux(highSideMux),
                _lowSideMux(lowSideMux),
                _pins(pins),
                _excitation(EXCITATION_VOLTAGE_5V) {}

    inline void setExcitationMode(StrainGauge_Excitation_e excitation) {
        _excitation = excitation;
    }

    int16_t read(void);
    float readMillivolts(void);

private:

    ADS114S0X* _adc;
    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    StrainGauge_Pinout_s _pins;
    StrainGauge_Excitation_e _excitation;
};