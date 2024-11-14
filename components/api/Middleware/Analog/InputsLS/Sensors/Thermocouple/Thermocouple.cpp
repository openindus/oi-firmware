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

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_B = {
    {-0.291, 2.431, {9.8423321E+01, 6.9971500E+02, -8.4765304E+02, 1.0052644E+03, -8.3345952E+02, 4.5508542E+02, -1.5523037E+02, 2.9886750E+01, -2.4742860E+00}},
    {2.431, 13.820, {2.1315071E+02, 2.8510642E+02, -5.2742887E+01, 2.9198024E+00, -1.2965030E+00, 1.2915072E-01, -6.6625199E-03, 1.8661696E-04, -2.4878585E-06}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_E = {
    {-8.825, 0.000, {0.0000000E+00, 1.6977288E+01, -4.3514970E+01, -1.5859697E-01, 9.2502871E-01, -2.6084314E-02, 4.1360199E-04, -3.4034030E-06, -1.1564890E-05}},
    {0.000, 76.373, {0.0000000E+00, 1.7057035E+01, -2.3301759E+01, 6.5435585E-03, -7.3562749E-05, -1.7896016E-06, 1.1603793E-08, -1.3735879E-10, 1.0629283E-11, -3.2447087E-14}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_J = {
    {-8.095, 0.000, {0.0000000E+00, 1.9528268E+01, -1.2286185E+00, -1.0752178E+00, -5.9086933E-01, -1.7256713E-01, -2.8131513E-02, -2.3963370E-03, -8.3823321E-05}},
    {0.000, 42.919, {0.0000000E+00, 1.9784250E+01, -2.0012040E-01, 1.0369690E-02, -2.5496870E-04, 3.5851530E-06, -5.3442850E-08, 5.0998900E-10}},
    {42.919, 69.553, {-3.1135819E+03, 3.0054368E+02, -9.9477323E+00, 1.7027663E-01, -1.4303347E-03, 4.7388608E-06}}
};

static const std::vector<TC_Coefficient_s> T_to_V_COEFF_K = {
    {-200.0, 0.0, {0.0000000E+00, 0.394501280250e-1, 0.236223735980e-4, -0.328589067840e-6, -0.499048287770e-8, -0.675090591730e-10, -0.574103274280e-12, -0.310888728940e-14, -0.104516093650e-16, -0.198892668780e-19}},
    {0.0, 1372.0, {-0.176004136860e-1, 0.389212049750e-1, 0.185587700320e-4, -0.994575928740e-7, 0.318409457190e-9, -0.560728448890e-12, 0.560750590590e-15, -0.320207200030e-18, 0.971511471520e-22, -0.121047212750e-25}},
};

static const std::vector<float> T_to_V_COEFF_K_A = {0.1185976, -0.1183432e-3, 0.1269686e+3};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_K = {
    {-5.891, 0.0, {0.0000000E+00, 2.5173462E+01, -1.1662878E+00, -1.0833638E+00, -8.9773540E-01, -3.7342377E-01, -8.6632643E-02, -1.0450598E-02, -5.1920577E-04}},
    {0.0, 20.644, {0.000000E+00, 2.508355E+01, 7.860106E-02, -2.503131E-01, 8.315270E-02, -1.228034E-02, 9.804036E-04, -4.413030E-05, 1.057734E-06, -1.052755E-08}},
    {20.644, 54.886, {-1.318058E+02, 4.830222E+01, -1.646031E+00, 5.464731E-02, -9.650715E-04, 8.802193E-06, -3.110810E-08}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_N = {
    {-3.990, 0.000, {0.0000000E+00, 3.8436847E+01, 1.1010485E+00, 5.2229312E+00, 7.2060525E+00, 5.8488586E+00, 2.7754916E+00, 7.7075166E-01, 1.1582665E-01, 7.3138868E-03}},
    {0.000, 20.613, {0.0000000E+00, 3.8689600E+01, -1.0826700E+00, 4.7020500E-02, -2.1216900E-06, -1.1727200E-04, 5.3928000E-06, -7.9815600E-08}},
    {20.613, 47.513, {1.9724850E+01, 3.3009430E+01, -3.9151590E-01, 9.8553910E-03, -1.2743710E-04, 7.7670220E-07}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_R = {
    {-0.226, 1.923, {0.0000000E+00, 1.8891380E+02, -9.3835290E+01, 1.3068619E+02, -2.2703580E+02, 3.5145659E+02, -3.8953900E+02, 2.8239471E+02, -1.2607281E+02, 3.1353611E+01, -3.3187769E+00}},
    {1.923, 13.228, {1.3345845E+01, 1.4726446E+02, -1.8440248E+01, 4.0311297E+00, -6.2494284E-01, 6.4684120E-02, -4.4587504E-03, 1.9947101E-04, -5.3134018E-06, 6.4819762E-08}},
    {13.228, 19.739, {-8.1995994E+01, 1.5539620E+02, -8.3421977E+00, 4.2794335E-01, -1.1915779E-02, 1.4922901E-04, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00}},
    {19.739, 21.103, {3.4061778E+04, -7.0237292E+03, 5.5829038E+02, -1.9523946E+01, 2.5607402E-01, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00, 0.0000000E+00}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_S = {
    {-0.235, 1.874, {0.00000000E+00, 1.84949460E+02, -8.00504062E+01, 1.02237430E+02, -1.52248592E+01, 1.88821343E+00, -1.59085941E-01, 8.23027880E-03}},
    {1.874, 11.950, {1.2915072E+01, 4.4668298E+01, -3.539139E+00, 3.149946E-01, -1.813197E-02, 3.187963E-04}},
    {10.332, 17.536, {-8.0878011E+01, 1.6215731E+02, -8.5368695E+00, 4.719686E-01, -1.441693E-02}},
    {17.536, 18.693, {5.3338751E+01, -1.2358923E+01, 1.092657E+00, -4.2656937E-02, 6.247205E-01}}
};

static const std::vector<TC_Coefficient_s> V_to_T_COEFF_T = {
    {-5.603, 0.0, {0.0000000E+00, 2.5949192E+1, -2.1316967E-1, 7.9018692E-1, 4.2527777E-1, 1.3304473E-1, 2.0241446E-2, 1.2668171E-3}},
    {0.0, 20.872, {0.000000E+00, 2.5928000E+1, -7.6029610E-1, 4.6377910E-2, -2.1653940E-3, 6.0481440E-5, -7.2934220E-7}}
};


float Thermocouple::_calculateVoltageFromTemperature(const std::vector<TC_Coefficient_s>& coefficients, const std::vector<float> coeffs_A, float temperature)
{
    for (const auto& coef : coefficients) {
        if (temperature >= coef.i && temperature <= coef.f) {
            // Calculate temperature using polynomial coefficients
            float voltage = 0.0;
            for (size_t i = 0; i < coef.d.size(); ++i) {
                voltage += coef.d[i] * std::pow(temperature, i);
            }
            voltage += coeffs_A[0] * exp(coeffs_A[1] * pow(temperature - coeffs_A[2], 2));;
            return voltage;
        }
    }

    ESP_LOGE(TAG, "Voltage is out of the range covered by the given coefficients.");
    return 0.0f;
}

float Thermocouple::_calculateTemperatureFromVoltage(const std::vector<TC_Coefficient_s>& coefficients, float voltage)
{
    for (const auto& coef : coefficients) {
        if (voltage >= coef.i && voltage <= coef.f) {
            // Calculate temperature using polynomial coefficients
            float temperature = 0.0;
            for (size_t i = 0; i < coef.d.size(); ++i) {
                temperature += coef.d[i] * std::pow(voltage, i);
            }
            return temperature;
        }
    }
    ESP_LOGE(TAG, "Voltage is out of the range covered by the given coefficients.");
    return 0.0f;
}

/**
 * @brief Read Voltage (mV)
 * 
 * @return float Voltage value
 */
float Thermocouple::readMillivolts(void)
{
    if (_adc == NULL) {
        ESP_LOGE(TAG, "%s() error", __func__);
        return 0.0f;
    }

    /* Set PGA Gain */
    _adc->setPGAGain(TC_GAIN_REGISTER);

    /* Set Internal reference to 2.5V */
    _adc->setReference(TC_ACQUISITION_REFERENCE);

    /* Set bias on negative input */
    _adc->setBias(static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));

    /* Set Internal mux */
    _adc->setInternalMux(static_cast<ads114s0x_adc_input_e>(_adcInputs[0]), static_cast<ads114s0x_adc_input_e>(_adcInputs[1]));

    /* Wait for stabilization if needed */
    _adc->waitStabilization();

    /* ADC Read */
    int16_t adcCode = _adc->read();
    
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
    float temperature = 0.0f;
    float cold_junction_voltage = 0.0f, voltage = readMillivolts();

    switch (_type)
    {
    case THERMOCOUPLE_B:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_B, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_E:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_E, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_J:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_J, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_K:
        cold_junction_voltage = _calculateVoltageFromTemperature(T_to_V_COEFF_K, T_to_V_COEFF_K_A, STDS75_get_temperature());
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_K, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_N:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_N, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_R:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_R, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_S:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_S, voltage + cold_junction_voltage);
        break;
    case THERMOCOUPLE_T:
        temperature = _calculateTemperatureFromVoltage(V_to_T_COEFF_T, voltage + cold_junction_voltage);
        break;
    default:
        ESP_LOGE(TAG, "Unknown sensor type");
        break;
    }
    
    return temperature;
}