/**
 * @file StrainGauge.cpp
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "StrainGauge.h"

static const char TAG[] = "StrainGauge";

// - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
float StrainGauge::read(bool print_result)
{
    int16_t adcCode = 0;

    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }
    adcCode = raw_read(0, 1);
    if (print_result) {
        // value result is a raw value between -256 and +256 (approximatively)
        // because of the division by SG_GAIN on the int16 
        print_float(((float) adcCode) / SG_GAIN, "/ 256 (raw value)");
    }
    return ((float) adcCode) / SG_GAIN;
}

void StrainGauge::update_strain_gauge_type(void)
{
    switch(_gauge_excitation_mode) {
    case EXCITATION_VOLTAGE_3V3:
        _mux_config.input = INPUT_SENSOR_3V3;
        _reference = SG_ACQUISITION_REFERENCE_VOLTAGE;
        _excitation = EXCITATION_OFF;
        break;
    case EXCITATION_VOLTAGE_5V:
        _mux_config.input = INPUT_SENSOR_5V;
        _reference = SG_ACQUISITION_REFERENCE_VOLTAGE;
        _excitation = EXCITATION_OFF;
        break;
    case EXCITATION_VOLTAGE_USER:
        _mux_config.input = INPUT_VISUP_DIV;
        _reference = SG_ACQUISITION_REFERENCE_VOLTAGE;
        _excitation = EXCITATION_OFF;
        break;
    case EXCITATION_CURRENT_10UA:
        _mux_config.input = INPUT_IDAC1;
        _reference = SG_ACQUISITION_REFERENCE_CURRENT;
        _excitation = EXCITATION_10_UA;
        break;
    case EXCITATION_CURRENT_100UA:
        _mux_config.input = INPUT_IDAC1;
        _reference = SG_ACQUISITION_REFERENCE_CURRENT;
        _excitation = EXCITATION_100_UA;
        break;
    case EXCITATION_CURRENT_500UA:
        _mux_config.input = INPUT_IDAC1;
        _reference = SG_ACQUISITION_REFERENCE_CURRENT;
        _excitation = EXCITATION_500_UA;
        break;
    case EXCITATION_CURRENT_1000UA:
        _mux_config.input = INPUT_IDAC1;
        _reference = SG_ACQUISITION_REFERENCE_CURRENT;
        _excitation = EXCITATION_1000_UA;
        break;
    case EXCITATION_CURRENT_2000UA:
        _mux_config.input = INPUT_IDAC1;
        _reference = SG_ACQUISITION_REFERENCE_CURRENT;
        _excitation = EXCITATION_2000_UA;
        break;
    }
}
