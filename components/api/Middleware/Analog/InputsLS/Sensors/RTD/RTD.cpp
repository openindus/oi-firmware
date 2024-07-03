/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"

int RTD::select(void)
{
    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        return -1;
    }

    /* MUX Excitation */
    _highSideMux->route(INPUT_IDAC1, _highSideMuxOutput);
    _lowSideMux->route(_lowSideMuxInput, OUTPUT_RBIAS_RTD);

    /* ADC */
    _adc->config(_adcInputP, _adcInputN);

    return 0;
}

int RTD::readRTD(std::vector<float>& rtd, uint32_t timeout_ms)
{
    if (_adc == NULL) {
        return -1;
    }

    _adc->clearData();

    int ret = 0;
    ret |= _adc->startConversion();
    ret |= _adc->autoCalibration();

    vTaskDelay(timeout_ms / portTICK_PERIOD_MS);

    ret |= _adc->stopConversion();

    rtd = _adc->readData();

    return ret;
}

int RTD::readTemperature(std::vector<float>& temp, uint32_t timeout_ms)
{
    if (_adc == NULL) {
        return -1;
    }

    std::vector<float> rtd;
    int ret = readRTD(rtd, timeout_ms);
    temp.resize(rtd.size());

    /* PT100 - Callendar-Van Dusen equation */
    const float R0 = 100.0;
    const float A = 3.9083e-3;
    const float B = -5.775e-7;
    const float C = -4.183e-12;
    
    for (int i=0; i<rtd.size(); i++) {
        if (rtd[i] < R0) {
            temp[i] = 9999999;
        } else {
            temp[i] = (-A + sqrt(A * A - (4 * B * (1 - (rtd[i] / R0))))) / (2 * B);
        }
    }

    return ret;
}
