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

/**
 * @brief Set the length of the acquisition in milliseconds
 * Default value is 50ms. If you set a shorter acquisition duration, the result will be less accurate.
 * 
 * @param duration Time in milliseconds
 * @return int 0 if success, -1 if error
 */
int AnalogInputsLS::setAcquisitionTime(AcquisitionDuration_e duration)
{
    int ret = 0;
    if (_adc != NULL) {
        ret |= _adc->setDataRate(static_cast<ads114s0x_data_rate_e>(duration));
    } else {
        ESP_LOGE(TAG, "Failed to set conversion time");
        ret = -1;
    }
    return ret;
}

/**
 * @brief Set the stabilization time before doing an acquisition
 * Default value is 0ms. 
 * But if your sensors need more time to stabilize before making an acquisition, you can add it with this function.
 * 
 * @param t Time in milliseconds
 * @return int 0 if success, -1 if error
 */
int AnalogInputsLS::setStabilizationTime(int t)
{
    int ret = 0;
    if (_adc != NULL) {
        ret |= _adc->setStabilizationTime(t);
    } else {
        ESP_LOGE(TAG, "Failed to set conversion time");
        ret = -1;
    }
    return ret;
}

/**
 * @brief Add sensor
 * 
 * @param type [RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE; STRAIN_GAUGE]
 * @param aIns Analog Inputs (AIN_A_P to AIN_E_N)
 * @return int the index of the added element (first call to this function for type RTD will return 0, second call 1, ...).
 *         return -1 in case of error
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
        case RAW_SENSOR:
            if (aIns.size() == 2) {
                raw.emplace_back(_adc, RawSensor_Pinout_s {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]});
                return raw.size()-1;
            } else {
                ESP_LOGE(TAG, "Raw Sensor requires 2 AINs.");
                return -1;
            }
            break;

        case RTD_TWO_WIRE:
            if (aIns.size() == 2) {
                rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                                RTD_Pinout_s {
                                    {AIN_TO_ADC_INPUT[aIns[0]], AIN_TO_ADC_INPUT[aIns[1]]},
                                    AIN_TO_MUX_IO[aIns[0]], 
                                    AIN_TO_MUX_IO[aIns[1]]
                                });
                return rtd.size()-1;
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
                                    AIN_TO_MUX_IO[aIns[2]]
                                });
                return rtd.size()-1;
            } else {
                ESP_LOGE(TAG, "RTD_THREE_WIRE requires 3 AINs.");
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
