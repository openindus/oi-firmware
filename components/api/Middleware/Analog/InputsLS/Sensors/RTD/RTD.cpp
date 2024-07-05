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
    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* MUX Excitation */
    _highSideMux->route(INPUT_IDAC1, _highSideMuxOutput);
    _lowSideMux->route(_lowSideMuxInput, OUTPUT_RBIAS_RTD);

    if (_nbWires == 2) {

    } else if (_nbWires == 3) {
        
    }

    /* ADC */
    _adc->config(_adcInputs[0], _adcInputs[1]);
    _adc->clearData();

    int ret = 0;
    ret |= _adc->startConversion();
    ret |= _adc->autoCalibration();

    vTaskDelay(timeout_ms / portTICK_PERIOD_MS);

    ret |= _adc->stopConversion();

    std::vector<uint16_t> adcCode;
    adcCode = _adc->readData();

    /* Calculate RTD resistor values */
    std::vector<float> rRtd;
    rRtd.resize(adcCode.size());
    for (int i=0; i<rRtd.size(); i++) {
        rRtd[i] = (float)(2 * ADS114S0X_R_REF * adcCode[i]) / 
            (float)(ADS114S0X_GAIN * (pow(2, ADS114S0X_RES) - 1));
    }

    /* Calculate the median */
    std::sort(rRtd.begin(), rRtd.end());
    size_t size = rRtd.size();
    if (size % 2 == 0) {
        return (rRtd[size / 2 - 1] + rRtd[size / 2]) / 2.0;
    } else {
        return rRtd[size / 2];
    }
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
