/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"

static const char TAG[] = "AnalogInputsLS";

static const ADC_Input_t AIN_TO_ADC_INPUT[] = {
    ADS114S0X_AIN5,
    ADS114S0X_AIN0,
    ADS114S0X_AIN8,
    ADS114S0X_AIN1,
    ADS114S0X_AIN9,
    ADS114S0X_AIN2,
    ADS114S0X_AIN10,
    ADS114S0X_AIN3,
    ADS114S0X_AIN11,
    ADS114S0X_AIN4
};

static const Multiplexer_IO_t AIN_TO_MUX_IO[] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4
};

std::vector<RTD> AnalogInputsLS::rtd;
std::vector<Thermocouple> AnalogInputsLS::tc;

ADS114S0X* AnalogInputsLS::_adc = NULL;
Multiplexer* AnalogInputsLS::_highSideMux = NULL;
Multiplexer* AnalogInputsLS::_lowSideMux = NULL;

int AnalogInputsLS::init(void)
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

int AnalogInputsLS::addSensor(Sensor_Type_t sensor, const std::vector<AIn_Num_t>& aIns)
{
    if (!std::all_of(aIns.begin(), aIns.end(), [](AIn_Num_t aIn) {
        return aIn >= AIN_A_P && aIn < AIN_MAX;
    })) {
        ESP_LOGE(TAG, "One or more AINs are out of range (0 to AIN_MAX - 1).");
        return -1;
    }

    switch (sensor) {
        case RTD_TWO_WIRE:
            if (aIns.size() == 2) {
                rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                    std::vector<ADC_Input_t>{AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]},
                    std::array<Multiplexer_IO_t, 2>{AIN_TO_MUX_IO[aIns[0]], AIN_TO_MUX_IO[aIns[1]]});
            } else {
                ESP_LOGE(TAG, "RTD_TWO_WIRE requires 2 AINs.");
                return -1;
            }
            break;
        case RTD_THREE_WIRE:
            if (aIns.size() == 3) {
                rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                    std::vector<ADC_Input_t>{AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]], AIN_TO_ADC_INPUT[aIns[2]]},
                    std::array<Multiplexer_IO_t, 2>{AIN_TO_MUX_IO[aIns[0]], AIN_TO_MUX_IO[aIns[1]]});
            } else {
                ESP_LOGE(TAG, "RTD_THREE_WIRE requires 3 AINs.");
                return -1;
            }
            break;
        case THERMOCOUPLE:
            if (aIns.size() == 2) {
                tc.emplace_back(_adc, std::array<ADC_Input_t, 2>{AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]});
            } else {
                ESP_LOGE(TAG, "THERMOCOUPLE requires 2 AINs.");
                return -1;
            }
            break;
        case STRAIN_GAUGE:
            if (aIns.size() == 4) {
                ESP_LOGW(TAG, "Not implemented !");
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
