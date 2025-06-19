/**
 * @file StrainGauge.cpp
 * @brief Strain gauge (current or voltage excitation)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "StrainGauge.h"

static const char TAG[] = "StrainGauge";

/**
 * @brief Read the strain gauge value
 * 
 * @param print_result true to print the result 
 * @return float the strain gauge value
 */
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

/**
 * @brief Set the parameter of the strain gauge
 * 
 * @param parameter the parameter to set
 * @param value the value to set
 */
void StrainGauge::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_ACQUISITION_TIME:
            setAcquisitionTime(value.acquisition_time);
            break;
        case PARAMETER_STABILIZATION_TIME:
            setStabilizationTime(value.stabilization_time);
            break;
        case PARAMETER_SG_EXCITATION:
            setSGExcitationMode(value.sg_excitation);
            break;
        default:
            ESP_LOGE(TAG, "The parameter you tried to modify is not accessible for this type of sensor.");
            break;
    }
}
