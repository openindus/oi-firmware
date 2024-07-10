/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"

#define RTD_R_REF 1000
#define RTD_GAIN 4

static const char TAG[] = "RTD";

float RTD::_calculateRTD(const std::vector<uint16_t>& adcCodes)
{
    /* Calculate RTD resistor values */
    std::vector<float> rRtds;
    rRtds.resize(adcCodes.size());
    for (int i=0; i<rRtds.size(); i++) {
        rRtds[i] = (float)(2 * RTD_R_REF * adcCodes[i]) / 
            (float)(RTD_GAIN * (pow(2, ADS114S0X_RESOLUTION) - 1));
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

/**
 * @brief Read RTD resistor (ohm)
 * 
 * @param timeMs Conversion time in milliseconds
 * @return float R_RTD value
 */
float RTD::readRTD(uint32_t timeMs)
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
    _adc->config(RTD_GAIN, REF_EXTERNAL_IDAC1, true);

    /* ADC Read */
    std::vector<uint16_t> adcCodes;
    if (_nbWires == 2) {
        _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], timeMs);
        rRTD = _calculateRTD(adcCodes);
    } else if (_nbWires == 3) {
        _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], timeMs / 2);
        float rRTD0 = _calculateRTD(adcCodes);
        _adc->read(&adcCodes, _adcInputs[2], _adcInputs[1], timeMs / 2);
        float rRTD1 = _calculateRTD(adcCodes);
        rRTD = std::abs(rRTD0 - rRTD1);
    } 

    /* MUX Excitation (disable) */
    _highSideMux->route(INPUT_OPEN_HS, _highSideMuxOutput);
    _lowSideMux->route(_lowSideMuxInput, OUTPUT_OPEN_LS);

    return rRTD;
}

/**
 * @brief Read temperature (°C)
 * 
 * @param timeMs Conversion time in milliseconds
 * @return float Temperature value
 */
float RTD::readTemperature(uint32_t timeMs)
{
    const float R0 = 100.0;
    const float A = 3.9083e-3;
    const float B = -5.775e-7;
    // const float C = -4.183e-12;

    float rRtd = readRTD(timeMs);

    /* PT100 - Callendar-Van Dusen equation */
    return (-A + sqrt(A * A - (4 * B * (1 - (rRtd / R0))))) / (2 * B);
}