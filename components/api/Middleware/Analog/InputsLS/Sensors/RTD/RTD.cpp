/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"

static const char TAG[] = "RTD";

float RTD::readRTD(uint32_t timeout_ms)
{
    float rRTD = 0.0;

    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* MUX Excitation */
    _highSideMux->route(INPUT_IDAC1, _highSideMuxOutput);
    _lowSideMux->route(_lowSideMuxInput, OUTPUT_RBIAS_RTD);

    /* ADC Config */
    _adc->config();

    /* ADC Read */
    std::vector<uint16_t> adcCodes;
    if (_nbWires == 2) {
        _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], timeout_ms);
        rRTD = _calculateRTD(adcCodes);
    } else if (_nbWires == 3) {
        _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], timeout_ms / 2);
        float rRTD0 = _calculateRTD(adcCodes);
        _adc->read(&adcCodes, _adcInputs[2], _adcInputs[1], timeout_ms / 2);
        float rRTD1 = _calculateRTD(adcCodes);
        rRTD = std::abs(rRTD0 - rRTD1);
    } 

    return rRTD;
}

float RTD::readTemperature(uint32_t timeout_ms)
{
    const float R0 = 100.0;
    const float A = 3.9083e-3;
    const float B = -5.775e-7;
    // const float C = -4.183e-12;

    float rRtd = readRTD(timeout_ms);

    /* PT100 - Callendar-Van Dusen equation */
    return (-A + sqrt(A * A - (4 * B * (1 - (rRtd / R0))))) / (2 * B);
}

float RTD::_calculateRTD(const std::vector<uint16_t>& adcCodes)
{
    /* Calculate RTD resistor values */
    std::vector<float> rRtds;
    rRtds.resize(adcCodes.size());
    for (int i=0; i<rRtds.size(); i++) {
        rRtds[i] = (float)(2 * ADS114S0X_R_REF * adcCodes[i]) / 
            (float)(ADS114S0X_GAIN * (pow(2, ADS114S0X_RES) - 1));
    }

    /* Calculate the median */
    std::sort(rRtds.begin(), rRtds.end());
    size_t size = rRtds.size();
    if (size % 2 == 0) {
        return (rRtds[size / 2 - 1] + rRtds[size / 2]) / 2.0;
    } else {
        return rRtds[size / 2];
    }
}