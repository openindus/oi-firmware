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

// Coefficients for Type K thermocouple
static const std::vector<float> COEF_K = 
    {0.0, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02 , 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08};

float Thermocouple::_calculateTemperature(const std::vector<float>& coefficients, float voltage)
{
    float temperature = 0.0;
    for (size_t i = 0; i < coefficients.size(); ++i) {
        temperature += coefficients[i] * pow(voltage, i);
    }
    return temperature;
}

/**
 * @brief Read Voltage (mV)
 * 
 * @param timeMs Conversion time in milliseconds
 * @return float Voltage value
 */
float Thermocouple::readVoltage(uint32_t timeMs)
{
    float voltage = 0.0;

    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* ADC Config */
    _adc->config(TC_GAIN, REF_INTERNAL_2V5, false);

    /* ADC Read */
    std::vector<uint16_t> adcCodes;
    _adc->read(&adcCodes, _adcInputs[0], _adcInputs[1], timeMs, true);

    /* Calculate Voltage values */
    std::vector<float> values;
    values.resize(adcCodes.size());
    for (int i=0; i<values.size(); i++) {
        values[i] = (float)(2 * TC_V_REF * adcCodes[i]) / 
            (float)(TC_GAIN * (pow(2, ADS114S0X_RESOLUTION) - 1));
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
 * @param timeMs Conversion time in milliseconds
 * @return float Temperature value
 */
float Thermocouple::readTemperature(uint32_t timeMs)
{
    float voltage = readVoltage(timeMs);
    return _calculateTemperature(COEF_K, voltage);
}