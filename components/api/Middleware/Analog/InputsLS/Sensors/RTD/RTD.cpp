/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"

#define RTD_R_REF                   1000
#define RTD_GAIN                    ADS114S0X_PGA_GAIN_4
#define RTD_EXCITATION_CURRENT      ADS114S0X_IDAC_1000_UA
#define RTD_ACQUISITION_REFERENCE   ADS114S0X_REF_REFP1_REFN1

static const char TAG[] = "RTD";

// Use this with table ? : https://github.com/drhaney/pt100rtd/blob/master/pt100rtd.cpp

float RTD::_calculateRTD(int adcCode)
{
    /* Calculate RTD resistor values */
    return (float)(2 * RTD_R_REF * adcCode) / (float)(RTD_GAIN * ADS114S0X_MAX_ADC_CODE);
}

/**
 * @brief Read RTD resistor (ohm)
 * 
 * @return float R_RTD value
 */
float RTD::readRTD(void)
{
    float rRTD = 0.0;

    if ((_highSideMux == NULL) || (_lowSideMux == NULL) || (_adc == NULL)) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* MUX Configuration:
     * Input excitation is IDAC1
     * Output should go to ground through the bias resistor for voltage offset */
    _highSideMux->route(INPUT_IDAC1, _hsMuxOutput);
    _lowSideMux->route(_lsMuxInput, OUTPUT_RBIAS_RTD);
    
    /* Set ADC Acquisition reference */
    _adc->setReference(RTD_ACQUISITION_REFERENCE);
  
    /* Set PGA Gain */
    _adc->setPGAGain(RTD_GAIN);

    /* Set excitation */
    _adc->setExcitation(RTD_EXCITATION_CURRENT);

    /* ADC Read */
    int adcCode;

    /* RTD 2 Wires */
    if (_adcInputs.size() == 2) 
    {
        /* Set internal mux */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));
        vTaskDelay(1000);
        adcCode = _adc->read();
        printf("ADCcode :%i\n", adcCode);
        rRTD = _calculateRTD(adcCode);
    } 
    /* RTD 3 Wires */
    else if (_adcInputs.size() == 3) 
    {
        /* Set internal mux */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));
        adcCode = _adc->read();
        float rRTD0 = _calculateRTD(adcCode);

        /* Set internal mux */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[1]), static_cast<ads114s0x_adc_input_e>(_adcInputs[2]));
        adcCode = _adc->read();
        float rRTD1 = _calculateRTD(adcCode);

        /* Subtract cable resistance to RTD resistance */
        rRTD = std::abs(rRTD0 - rRTD1);
    } 

    /* Stop exitation */
    _adc->setExcitation(ADS114S0X_IDAC_OFF);

    /* Reset Internal MUX */
    _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);

    /* MUX Configuration (IMPORTANT !!):
     * Input excitation IDAC1 should go to GND for discharge
     * Output should go to ground for discharge */
    _highSideMux->route(INPUT_IDAC1, 5); // 5 is connected to GND (since AE02)
    _lowSideMux->route(_lsMuxInput, OUTPUT_GND);

    return rRTD;
}

/**
 * @brief Read temperature (°C)
 * 
 * @return float Temperature value
 */
float RTD::readTemperature(void)
{
    const float R0 = 100.0;
    const float A = 3.9083e-3;
    const float B = -5.775e-7;
    // const float C = -4.183e-12;

    float rRtd = readRTD();
    printf("res:%f\n", rRtd);
    /* PT100 - Callendar-Van Dusen equation */
    return ((-A + sqrt(A * A - (4 * B * (1 - (rRtd / R0))))) / (2 * B));
}