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

/**
 * @brief Set conversion time (ms)
 * 
 * @param t Time in milliseconds
 * @return int 0 if success, -1 if error
 */
int AnalogInputsLS::setConversionTime(uint32_t t)
{
    if (_adc != NULL) {
        _adc->setConvTimeMs(t);
        return 0;
    } else {
        ESP_LOGE(TAG, "Failed to set conversion time");
        return -1;
    }
}

/**
 * @brief Add sensor
 * 
 * @param type [RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE; STRAIN_GAUGE]
 * @param aIns Analog Inputs (AIN_A_P to AIN_E_N)
 * @return int 0 if success, -1 if error
 */
int AnalogInputsLS::addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns)
{
    if (!std::all_of(aIns.begin(), aIns.end(), [](AIn_Num_t aIn) {
        return aIn >= AIN_A_P && aIn < AIN_MAX;
    })) {
        ESP_LOGE(TAG, "One or more AINs are out of range (0 to AIN_MAX - 1).");
        return -1;
    }

    switch (type) {
        case RTD_TWO_WIRE:
            if (aIns.size() == 2) {
                rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                    RTD_Pinout_s {
                        {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]},
                        AIN_TO_MUX_IO[aIns[0]], 
                        AIN_TO_MUX_IO[aIns[1]]});
            } else {
                ESP_LOGE(TAG, "RTD_TWO_WIRE requires 2 AINs.");
                return -1;
            }
            break;
        case RTD_THREE_WIRE:
            if (aIns.size() == 3) {
                rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                    RTD_Pinout_s {
                        {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]], AIN_TO_ADC_INPUT[aIns[2]]},
                        AIN_TO_MUX_IO[aIns[0]], 
                        AIN_TO_MUX_IO[aIns[1]]});
            } else {
                ESP_LOGE(TAG, "RTD_THREE_WIRE requires 3 AINs.");
                return -1;
            }
            break;
        case THERMOCOUPLE:
            if (aIns.size() == 2) {
                tc.emplace_back(_adc, 
                    TC_Pinout_s {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]});
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
                        AIN_TO_ADC_INPUT[aIns[2]], AIN_TO_ADC_INPUT[aIns[2]],
                        AIN_TO_MUX_IO[aIns[0]], AIN_TO_MUX_IO[aIns[1]]});
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
