/**
 * @file Thermocouple.cpp
 * @brief Thermocouple
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Thermocouple.h"

#define TC_V_REF                    2.5
#define TC_GAIN                     8
#define TC_GAIN_REGISTER            ADS114S0X_PGA_GAIN_8
#define TC_ACQUISITION_REFERENCE    ADS114S0X_REF_INTERNAL_2_5V

static const char TAG[] = "Thermocouple";

/* Thermocouple coefficients  */
static const std::vector<TC_Coefficient_s> COEFF_K = {
    {-200.0, 0.0, -5.891, 0.0, {0.0000000E+00, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01, -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04, 0.0000000E+00}},
    {0.0, 500.0, 0.0, 20.644, {0.000000E+00, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08}},
    {500.0, 1372.0, 20.644, 54.886, {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02, -9.650715E-04, 8.802193E-06, -3.110810E-08, 0.000000E+00, 0.000000E+00, 0.000000E+00}}
};

static const std::vector<TC_Coefficient_s> COEFF_T = {
    {-200.0, 0.0, -5.603, 0.0, {0.0000000E+00, 2.5949192E+1, -2.1316967E-1, 7.9018692E-1, 4.2527777E-1, 1.3304473E-1, 2.0241446E-2, 1.2668171E-3}},
    {0.0, 400.0, 0.0, 20.872, {0.000000E+00, 2.5928000E+1, -7.6029610E-1, 4.6377910E-2, -2.1653940E-3, 6.0481440E-5, -7.2934220E-7}}
};

float Thermocouple::_calculateTemperature(const std::vector<TC_Coefficient_s>& coefficients, float voltage)
{
    for (const auto& coef : coefficients) {
        if (voltage >= coef.Ei && voltage <= coef.Ef) {
            // Calculate temperature using polynomial coefficients
            float temperature = 0.0;
            for (size_t i = 0; i < coef.d.size(); ++i) {
                temperature += coef.d[i] * std::pow(voltage, i);
            }
            return temperature;
        }
    }
    ESP_LOGE(TAG, "Voltage is out of the range covered by the given coefficients.");
    return 0.0;
}

/**
 * @brief Read Voltage (mV)
 * 
 * @return float Voltage value
 */
float Thermocouple::readVoltage(void)
{
    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* MUX Configuration:
     * Disconnect input
     * Disconnect output */
    _highSideMux->route(INPUT_OPEN_HS, 7); // 7 is not connected to any channel
    _lowSideMux->route(7, OUTPUT_OPEN_LS); // 7 is not connected to any channel

    /* Set PGA Gain */
    _adc->setPGAGain(TC_GAIN_REGISTER);

    /* Set Internal reference to 2.5V */
    _adc->setReference(TC_ACQUISITION_REFERENCE);

    /* Set bias on negative input */
    _adc->setBias(static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));

    /* Set Internal mux */
    _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));
    vTaskDelay(100);
    /* ADC Read */
    int adcCode = _adc->read();
    printf("ADCCode: %i\n", adcCode);
    
    /* Stop Vbias */
    _adc->setBias(ADS114S0X_NOT_CONNECTED);

    /* Reset Internal MUX */
    _adc->setInternalMux(ADS114S0X_NOT_CONNECTED, ADS114S0X_NOT_CONNECTED);

    /* Calculate Voltage values */
    float value = (float)(2 * TC_V_REF * adcCode) / (float)(TC_GAIN * ADS114S0X_MAX_ADC_CODE);

    return value * 1000;
}

/**
 * @brief Read temperature (°C)
 * 
 * @return float Temperature value
 */
float Thermocouple::readTemperature(void)
{
    float temperature = 0.0;
    float voltage = readVoltage();
    printf("TC%i voltage :%f\n", _type, voltage);
    switch (_type)
    {
    case THERMOCOUPLE_K:
        temperature = _calculateTemperature(COEFF_K, voltage);
        break;
    case THERMOCOUPLE_T:
        temperature = _calculateTemperature(COEFF_T, voltage);
        break;
    default:
        ESP_LOGE(TAG, "Unknown sensor type");
        break;
    }
    
    return temperature;
}