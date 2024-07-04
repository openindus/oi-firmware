/**
 * @file AnalogInputsLS.cpp
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsLS.h"

static const char TAG[] = "AnalogInputsLS";

static const ADC_Input_t AINLS_TO_ADC_INPUT[] = {
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

static const Multiplexer_IO_t AINLS_TO_MUX_IO[] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4
};

std::vector<RTD> AnalogInputsLS::rtd;

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

int AnalogInputsLS::addSensor(Sensor_Type_t sensor, AInLS_Num_t ainP, AInLS_Num_t ainN)
{
    if (sensor == RTD_TWO_WIRE) {
        if (ainP < AINLS_MAX && ainN < AINLS_MAX) {
            rtd.emplace_back(_adc, _highSideMux, _lowSideMux, 
                AINLS_TO_ADC_INPUT[ainP], AINLS_TO_ADC_INPUT[ainN],
                AINLS_TO_MUX_IO[ainP], AINLS_TO_MUX_IO[ainN]);
        } else {
            ESP_LOGE(TAG, "Unable to add an RTD");
            return -1;
        }  
    } else {
        ESP_LOGE(TAG, "Sensor not implemented");
    }
    return 0;
}

int AnalogInputsLS::addSensor(Sensor_Type_t sensor, AInLS_Num_t ainP0, AInLS_Num_t ainN0, AInLS_Num_t ainN1)
{
    return -1;
}
