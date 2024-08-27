/**
 * @file Thermocouple.cpp
 * @brief Thermocouple
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Thermocouple.h"

#define TC_GAIN 8
#define TC_V_REF 2.5

static const char TAG[] = "Thermocouple";

/* Thermocouple coefficients  */
static const std::vector<TC_Coefficient_s> COEF_K = {
    {-200.0, 0.0, -5.891, 0.0, {0.0000000E+00, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01, -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04, 0.0000000E+00}},
    {0.0, 500.0, 0.0, 20.644, {0.000000E+00, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08}},
    {500.0, 1372.0, 20.644, 54.886, {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02, -9.650715E-04, 8.802193E-06, -3.110810E-08, 0.000000E+00, 0.000000E+00, 0.000000E+00}}
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
    float voltage = 0.0;

    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* ADC Config */
    _adc->config(static_cast<ADS114S0X_Gain_e>(TC_GAIN), REF_INTERNAL_2V5, false);

    /* ADC Read */
    std::vector<uint16_t> adcCodes;
    _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], true);

    /* Calculate Voltage values */
    std::vector<float> values;
    values.resize(adcCodes.size());
    for (int i=0; i<values.size(); i++) {
        values[i] = (float)(2 * TC_V_REF * adcCodes[i]) / 
            (float)(TC_GAIN * (pow(2, ADS114S0X_RESOLUTION) - 1));
        printf("%d\r\n",adcCodes[i]);
        printf("%f\r\n",values[i]);
    }

    /* Calculate the median */
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    if (size % 2 == 0) {
        voltage = (values[size / 2 - 1] + values[size / 2]) / 2.0;
    } else {
        voltage = values[size / 2];
    }

    return voltage * 1000;
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

    switch (_type)
    {
    case TYPE_K:
        temperature = _calculateTemperature(COEF_K, voltage);
        break;
    
    default:
        ESP_LOGE(TAG, "Unknown sensor type");
        break;
    }
    
    return temperature;
}