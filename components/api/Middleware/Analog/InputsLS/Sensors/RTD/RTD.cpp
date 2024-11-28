/**
 * @file RTD.cpp
 * @brief RTD (PT100 / PT1000)
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RTD.h"

#define RTD_R_REF                       3000
#define RTD_PT100_GAIN                  8
#define RTD_PT1000_GAIN                 1
#define RTD_PT100_GAIN_REGISTER         ADS114S0X_PGA_GAIN_8
#define RTD_PT1000_GAIN_REGISTER        ADS114S0X_PGA_GAIN_1
#define RTD_PT100_EXCITATION_CURRENT    ADS114S0X_IDAC_750_UA
#define RTD_PT1000_EXCITATION_CURRENT   ADS114S0X_IDAC_250_UA
#define RTD_ACQUISITION_REFERENCE       ADS114S0X_REF_REFP1_REFN1

static const char TAG[] = "RTD";

// Value of temperature for pt100 by decade
// First value is the value for 10ohms and last one is the value for 400Ohms
// A zero is added at the beginning to match index and resistor value
// Value are taken from online fluke pt100 calculator
static const float pt100_table[] = {-242.021, -219.539, -196.572, -173.158, -149.335, -125.146, -100.631, -75.828, -50.771, -25.488,
                                    0.000, 25.684, 51.566, 77.651, 103.943, 130.447, 157.169, 184.115, 211.289, 238.698, 
                                    266.348, 294.246, 322.397, 350.810, 379.492, 408.450, 437.693, 467.229, 497.067, 527.217, 
                                    557.688, 588.491, 619.638, 651.140, 683.009, 715.259, 747.903, 780.957, 814.436, 848.357, 
                                    882.737};
    

float RTD::_calculateRTD(int16_t adcCode)
{
    /* Calculate RTD resistor values */
    if (_type == RTD_PT100)
        return (float)(2 * RTD_R_REF * adcCode) / (float)(RTD_PT100_GAIN * ADS114S0X_MAX_ADC_CODE);
    else if (_type == RTD_PT1000) {
        return (float)(2 * RTD_R_REF * adcCode) / (float)(RTD_PT1000_GAIN * ADS114S0X_MAX_ADC_CODE);
    }
    return 0.0f;
}

/**
 * @brief Read RTD resistor (ohm)
 * 
 * @return float R_RTD value
 */
float RTD::readResistor(void)
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
    /* Set excitation */
    if (_type == RTD_PT100) {
        _adc->setExcitation(RTD_PT100_EXCITATION_CURRENT);
        _adc->setPGAGain(RTD_PT100_GAIN_REGISTER);
    } else if (_type == RTD_PT1000) {
        _adc->setExcitation(RTD_PT1000_EXCITATION_CURRENT);
        _adc->setPGAGain(RTD_PT1000_GAIN_REGISTER);
    }

    /* Wait for stabilization if needed */
    _adc->waitStabilization();

    int16_t adcCode;
    /* RTD 2 Wires */
    if (_adcInputs.size() == 2) 
    {
        /* Set internal mux */
        _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));
        adcCode = _adc->read();
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

    /* MUX Configuration:
     * Disconnect input
     * Disconnect output */
    _highSideMux->route(INPUT_OPEN_HS, _hsMuxOutput);
    _lowSideMux->route(_lsMuxInput, OUTPUT_OPEN_LS);

    return rRTD;
}

/**
 * @brief Read temperature (°C)
 * 
 * @return float Temperature value
 */
float RTD::readTemperature(void)
{
    // Read resistor value
    float rRtd = readResistor();
    float temperature = NAN;

    if ((_type == RTD_PT100 && rRtd >= 10 && rRtd <= 390) || (_type == RTD_PT1000 && rRtd >= 100 && rRtd <= 2900))
    {
        // Second order interpolation
        if (_type == RTD_PT100) rRtd = rRtd / 10.0;
        if (_type == RTD_PT1000) rRtd = rRtd / 100.0;
        int integer = floor(rRtd);
        float decimal = rRtd - integer;

        float a = pt100_table[integer];
        float b = pt100_table[integer + 1] / 2;
        float c = pt100_table[integer - 1] / 2;
        temperature = a + decimal * (b - c + decimal * (c + b - a));
    }

    return temperature;
}