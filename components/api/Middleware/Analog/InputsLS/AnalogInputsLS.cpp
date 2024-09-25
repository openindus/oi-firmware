/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"

static const char TAG[] = "AnalogInputsLS";

static const std::array<ADC_Input_t, 10> AIN_TO_ADC_INPUT = {
    5, 0, 8, 1, 9, 2, 10, 3, 11, 4
};

static const std::array<Mux_IO_t, 10> AIN_TO_MUX_IO = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4
};

ADS114S0X* AnalogInputsLS::_adc = NULL;
Multiplexer* AnalogInputsLS::_highSideMux = NULL;
Multiplexer* AnalogInputsLS::_lowSideMux = NULL;

int AnalogInputsLS::_init(void)
{
    int ret = 0;

    /* ADC */
    if (_adc != NULL) {
        ret |= _adc->init();
    } else {
        ESP_LOGE(TAG, "Failed to initialize ADC device");
        ret |= -1;
    }

    /* Multiplexer (high side/ low side)*/
    if ((_highSideMux != NULL) && (_lowSideMux != NULL)) {
        ret |= _highSideMux->init();
        ret |= _lowSideMux->init();
    } else {
        ESP_LOGE(TAG, "Failed to initialize multiplexer");
        ret |= -1;
    }

    /* Digipot */

    /* Digital thermometer */

    return ret;
}

std::vector<RTD> AnalogInputsLS::rtd;
std::vector<Thermocouple> AnalogInputsLS::tc;
std::vector<StrainGauge> AnalogInputsLS::sg;
std::vector<RawSensor> AnalogInputsLS::raw;

void AnalogInputsLS::setAcquisitionTime(AcquisitionDuration_e duration)
{
    if (_adc != NULL) {
        _adc->setDataRate(static_cast<ads114s0x_data_rate_e>(duration));
    } else {
        ESP_LOGE(TAG, "Failed to set conversion time");
    }
}

void AnalogInputsLS::setStabilizationTime(int t)
{
    if (_adc != NULL) {
        _adc->setStabilizationTime(t);
    } else {
        ESP_LOGE(TAG, "Failed to set conversion time");
    }
}

int AnalogInputsLS::addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns)
{
    if (!std::all_of(aIns.begin(), aIns.end(), [](AIn_Num_t aIn) {
        return aIn >= AIN_A_P && aIn < AIN_MAX;
    })) {
        ESP_LOGE(TAG, "One or more AINs are out of range (0 to AIN_MAX - 1).");
        return -1;
    }

    switch (type) {

        case RAW_SENSOR:
            if (aIns.size() == 2) {
                raw.emplace_back(_adc, RawSensor_Pinout_s {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]});
                return raw.size()-1;
            } else {
                ESP_LOGE(TAG, "Raw Sensor requires 2 AINs.");
                return -1;
            }
            break;

        case RTD_PT100:
        case RTD_PT1000:
            if (aIns.size() == 2) {
                rtd.emplace_back(_adc, type, _highSideMux, _lowSideMux, 
                                RTD_Pinout_s {
                                    {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]},
                                    AIN_TO_MUX_IO[aIns[0]], 
                                    AIN_TO_MUX_IO[aIns[1]]
                                });
                return rtd.size()-1;
            } else if (aIns.size() == 3) {
                rtd.emplace_back(_adc, type, _highSideMux, _lowSideMux, 
                                RTD_Pinout_s {
                                    {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]], AIN_TO_ADC_INPUT[aIns[2]]},
                                    AIN_TO_MUX_IO[aIns[0]], 
                                    AIN_TO_MUX_IO[aIns[2]]
                                });
                return rtd.size()-1;
            } else {
                ESP_LOGE(TAG, "RTD type requires 2 or 3 AINs.");
                return -1;
            }
            break;

        case THERMOCOUPLE_B:
        case THERMOCOUPLE_E:
        case THERMOCOUPLE_J:
        case THERMOCOUPLE_K:
        case THERMOCOUPLE_N:
        case THERMOCOUPLE_R:
        case THERMOCOUPLE_S:
        case THERMOCOUPLE_T:
            if (aIns.size() == 2) {
                tc.emplace_back(_adc, TC_Pinout_s {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]}, type);
                return tc.size()-1;
            } else {
                ESP_LOGE(TAG, "THERMOCOUPLE requires 2 AINs.");
                return -1;
            }
            break;

        case STRAIN_GAUGE:
            if (aIns.size() == 4) {
                sg.emplace_back(_adc, _highSideMux, _lowSideMux,
                                StrainGauge_Pinout_s { 
                                    AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]],
                                    AIN_TO_ADC_INPUT[aIns[2]], AIN_TO_ADC_INPUT[aIns[3]],
                                    AIN_TO_MUX_IO[aIns[2]], AIN_TO_MUX_IO[aIns[3]]
                                });
                return sg.size()-1;
            } else {
                ESP_LOGE(TAG, "STRAIN_GAUGE requires 4 AINs.");
                return -1;
            }
            break;

        default:
            ESP_LOGE(TAG, "Unknown sensor type.");
    }
    return 0;
}
