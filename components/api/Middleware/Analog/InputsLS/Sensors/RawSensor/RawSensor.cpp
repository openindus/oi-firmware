/**
 * @file RawSensor.cpp
 * @brief RawSensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RawSensor.h"

#define V_REF_MILLIVOLTS    2500.0f

static const char TAG[] = "RawSensor";

/**
 * @brief Read raw value
 * 
 * @return int16_t adcCode
 */
int16_t RawSensor::read(void)
{
    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* Set PGA Gain */
    _adc->setPGAGain((ads114s0x_pga_gain_e)_gain);

    /* Set Internal reference to 2.5V */
    _adc->setReference((ads114s0x_ref_selection_e)_reference);
// - [X] TASK set the right reference

    /* Set bias on negative input */
    _adc->setBias(static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));

    /* Set Internal mux */
    _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));

    /* Wait for stabilization if needed */
    _adc->waitStabilization();

    /* ADC Read */
    int16_t adcCode = _adc->read();
    printf("ADCCode: %i\n", adcCode);
    
    /* Stop Vbias */
    // _adc->setBias(ADS114S0X_NOT_CONNECTED);

    /* Reset Internal MUX */
    // _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);

    return adcCode;
}

/**
 * @brief Read differential input voltage
 * 
 * @return float Voltage in millivolts
 */
float RawSensor::readMillivolts(void)
{
    int16_t adcCode = read();
    printf("adc: %i\n", adcCode);
    return (2 * V_REF_MILLIVOLTS*(float)adcCode)/(float)(std::pow(2,(int)_gain)*ADS114S0X_MAX_ADC_CODE);
}