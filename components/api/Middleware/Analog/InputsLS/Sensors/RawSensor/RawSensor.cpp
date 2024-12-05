/**
 * @file RawSensor.cpp
 * @brief RawSensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RawSensor.h"

#define V_REF_MILLIVOLTS    2500.0f

static const char TAG[] = "RawSensor";

/**
 * @brief Read differential input voltage
 * 
 * @return float Voltage in millivolts
 */
// - [X] TASK make read commands accept a defaulted to false parameter that describes if the return value should be printed
float RawSensor::readMillivolts(bool print_result)
{
    int16_t adcCode = raw_read();
    float value = (2 * V_REF_MILLIVOLTS*(float)adcCode)/(float)(std::pow(2,(int)_gain)*ADS114S0X_MAX_ADC_CODE);

    if (print_result) {
        // result is a float value
        print_float(value, "(raw value)");
    }
    return value;
}

void RawSensor::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_MUX_HS_INDEX:
        case PARAMETER_MUX_HS_INPUT:
        case PARAMETER_MUX_LS_INDEX:
        case PARAMETER_MUX_LS_OUTPUT:
        setMuxParameter(parameter, value.mux_parameter);
        break;
        case PARAMETER_BIAS:
        setBiasActive(value.bias);
        break;
        case PARAMETER_GAIN:
        setGain(value.gain);
        break;
        case PARAMETER_REFERENCE:
        setReference(value.reference);
        break;
        case PARAMETER_EXCITATION_MODE:
        setExcitation(value.excitation_mode);
        break;
        default:
        printf("The parameter you tried to modify is not accessible for this type of sensor.");
    }
}

void RawSensor::setMuxParameter(Sensor_Parameter_e parameter, Mux_Parameter_u value)
{
    switch (parameter) {
        case PARAMETER_MUX_HS_INDEX:
        _mux_config.hs_index = value.hs_index;
        break;
        case PARAMETER_MUX_HS_INPUT:
        _mux_config.input = value.input;
        break;
        case PARAMETER_MUX_LS_INDEX:
        _mux_config.ls_index = value.ls_index;
        break;
        case PARAMETER_MUX_LS_OUTPUT:
        _mux_config.output = value.output;
        default:
        break;
    }
}
