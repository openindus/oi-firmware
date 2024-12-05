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
#include "global_sensor.hpp"

#define SG_GAIN                             128.0f
#define SG_GAIN_REGISTER                    GAIN_128
#define SG_ACQUISITION_REFERENCE_VOLTAGE    REFERENCE_EXCITATION
#define SG_ACQUISITION_REFERENCE_CURRENT    REFERENCE_IDAC_1

class StrainGauge: public Sensor
{
public:

    StrainGauge(ADS114S0X* adc, Multiplexer* highSideMux, Multiplexer* lowSideMux, Sensor_Pinout_s pinout, uint32_t index) :
        Sensor(adc, highSideMux, lowSideMux, pinout, STRAIN_GAUGE, index),
        _gauge_excitation_mode(EXCITATION_VOLTAGE_5V)
    {
        _gain = SG_GAIN_REGISTER;
        _mux_config.output = OUTPUT_RBIAS_GAUGE;
        _mux_config.hs_index = 2;
        _mux_config.ls_index = 3;
    }

    inline void setSGExcitationMode(StrainGauge_Excitation_e excitation) {
        _gauge_excitation_mode = excitation;
        update_strain_gauge_type();
    }
    // - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
    float read(bool print_result = false);

protected:
    void update_strain_gauge_type(void);

private:
    StrainGauge_Excitation_e _gauge_excitation_mode;
};
