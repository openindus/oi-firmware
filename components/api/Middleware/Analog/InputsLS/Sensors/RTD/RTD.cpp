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
    _highSideMux->route(1, _highSideMuxOutput); // IDAC1 to AINLS_P
    _lowSideMux->route(_lowSideMuxInput, 3); // AINLS_N to RBIAS RTD

    /* ADC */
    _adc->config();

    return 0;
}

int RTD::readRTD(std::vector<float>* rtd, uint32_t timeout_ms)
{
    if (_adc == NULL) {
        return -1;
    }

    int ret = 0;
    ret |= _adc->clearData();
    ret |= _adc->startConversion();
    vTaskDelay(500 / portTICK_PERIOD_MS);

    ret |= _adc->autoCalibration();
    vTaskDelay(timeout_ms / portTICK_PERIOD_MS);

    ret |= _adc->stopConversion();
    vTaskDelay(500 / portTICK_PERIOD_MS);

    ret |= _adc->readData(rtd);

    return ret;
}


int RTD::readTemperature(std::vector<float>* temp, uint32_t timeout_ms)
{
    return 0;
}
