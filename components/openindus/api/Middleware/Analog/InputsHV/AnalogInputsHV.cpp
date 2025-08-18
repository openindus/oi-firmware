/**
 * @file AnalogInputsHV.cpp
 * @brief Analog Inputs High Voltage
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputsHV.h"

static const char TAG[] = "AnalogInputsHV";

adc_unit_t AnalogInputsHV::_adc_unit;
adc_channel_t* AnalogInputsHV::_adc_channels;
uint8_t AnalogInputsHV::_nb_channels;
adc_oneshot_unit_handle_t* AnalogInputsHV::_adc_handles;
adc_cali_handle_t* AnalogInputsHV::_adc_cali_handles;
float* AnalogInputsHV::_coeff_a;
float* AnalogInputsHV::_coeff_b;

int AnalogInputsHV::init(const adc_unit_t adc_unit,
    const adc_channel_t* adc_channels,
    uint8_t nb_channels,
    adc_oneshot_unit_handle_t adc_handle) 
{
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "Initialize Analog Inputs");

    _nb_channels = nb_channels;
    _adc_channels = (adc_channel_t*)calloc(_nb_channels, sizeof(adc_channel_t));
    _adc_handles = (adc_oneshot_unit_handle_t*)calloc(_nb_channels, sizeof(adc_oneshot_unit_handle_t));
    _adc_cali_handles = (adc_cali_handle_t*)calloc(_nb_channels, sizeof(adc_cali_handle_t));
    _coeff_a = (float*)calloc(_nb_channels, sizeof(float));
    _coeff_b = (float*)calloc(_nb_channels, sizeof(float));

    for (size_t i = 0; i < nb_channels; i++) {
        _adc_channels[i] = adc_channels[i];
        _adc_handles[i] = adc_handle;
        _adc_cali_handles[i] = NULL;
        
        // Configure ADC channel
        adc_oneshot_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_oneshot_config_channel(_adc_handles[i], adc_channels[i], &config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure ADC channel for input %d", i);
            continue;
        }

        // Initialize ADC calibration
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = adc_unit,
            .chan = adc_channels[i],
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &_adc_cali_handles[i]);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create ADC calibration scheme for input %d", i);
        }

        // Get eFuse coefficients
        if (esp_efuse_block_is_empty(EFUSE_BLK_KEY4) == false) {
            // getting coeff in eFuse
            struct {
                float ain_coeff_a;
                float ain_coeff_b;
            } temp_coeff;
            esp_efuse_read_block(EFUSE_BLK_KEY4, &temp_coeff, sizeof(temp_coeff)*8*i, sizeof(temp_coeff)*8);
            // Checking if coeff are valid
            if ((abs(temp_coeff.ain_coeff_a) < 100.0f) && (abs(temp_coeff.ain_coeff_b) < 1000.0f)) {
                _coeff_a[i] = temp_coeff.ain_coeff_a;
                _coeff_b[i] = temp_coeff.ain_coeff_b;
                ESP_LOGI(TAG, "Reading AIN_%i coeffs: a=%f b=%f", i+1, _coeff_a[i], _coeff_b[i]);
            } else {
                _coeff_a[i] = ESP_ADC_DEFAULT_COEFF_A;
                _coeff_b[i] = ESP_ADC_DEFAULT_COEFF_B;
                ESP_LOGI(TAG, "Reading default AIN_%i coeffs: a=%f b=%f", i+1, _coeff_a[i], _coeff_b[i]);
            }
        } else {
            _coeff_a[i] = ESP_ADC_DEFAULT_COEFF_A;
            _coeff_b[i] = ESP_ADC_DEFAULT_COEFF_B;
            ESP_LOGI(TAG, "Reading default AIN_%i coeffs: a=%f b=%f", i+1, _coeff_a[i], _coeff_b[i]);
        }
    }

    _registerCLI();

    return 0;
}

float AnalogInputsHV::read(AnalogInput_Num_t num, AnalogInput_Unit_t unit)
{
    if (num >= _nb_channels) {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
        return -1;
    }

    // Read raw ADC values with averaging
    int voltage_sum = 0;
    int adc_raw = 0;
    int voltage = 0;
    
    // Making the mean with voltage and not adc_reading to be more precise
    for (int i = 0; i < ESP_ADC_NO_OF_SAMPLES; i++)
    {
        esp_err_t ret = adc_oneshot_read(_adc_handles[num], _adc_channels[num], &adc_raw);
        if (ret == ESP_OK) {
            if (_adc_cali_handles[num] != NULL) {
                ret = adc_cali_raw_to_voltage(_adc_cali_handles[num], adc_raw, &voltage);
                if (ret == ESP_OK) {
                    voltage_sum += voltage;
                }
            }
        }
    }
    
    float avg_voltage = (float) voltage_sum / (float) ESP_ADC_NO_OF_SAMPLES;
    
    // Apply unit conversion
    float value = 0.0f;
    switch (unit) {
        case AIN_UNIT_RAW:
            value = avg_voltage;
            break;
        case AIN_UNIT_VOLT:
            // Return 0V if voltage is too low
            if (avg_voltage < 0.1f) {
                value = avg_voltage / 1000.0f;
            } else {
                value = (avg_voltage * _coeff_a[num] + _coeff_b[num]) / 1000.0f;
            }
            break;
        case AIN_UNIT_MILLIVOLT:
            // Return 0mV if voltage is too low
            if (avg_voltage < 0.1f) {
                value = avg_voltage;
            } else {
                value = avg_voltage * _coeff_a[num] + _coeff_b[num];
            }
            break;
        default:
            ESP_LOGE(TAG, "Undefined unit");
            break;
    }
    return value;
}

float AnalogInputsHV::analogRead(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_RAW);
}

float AnalogInputsHV::analogReadVolt(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_VOLT);
}

float AnalogInputsHV::analogReadMilliVolt(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_MILLIVOLT);
}

int AnalogInputsHV::setAnalogCoeffs(float* a, float* b)
{
    ESP_LOGW(TAG, "This operation can be done only once !");

    struct {
        float ain_coeff_a;
        float ain_coeff_b;
    } coeffs[_nb_channels];

    for (int i = 0; i < _nb_channels; i++) {

        // Fill data with coeffs
        if ((abs(a[i]) < 100.0f) && (abs(b[i]) < 1000.0f)) {
            ESP_LOGI(TAG, "Setting AIN_%i coeffs: a=%f b=%f", i+1, a[i], b[i]);
            coeffs[i].ain_coeff_a = a[i];
            coeffs[i].ain_coeff_b = b[i];
        } 
        else {
            ESP_LOGE(TAG, "Invalid coefficients for AnalogInputsHV");
            return -1;
        }
    }

    // Write coeff into eFuse
    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY4, ESP_EFUSE_KEY_PURPOSE_USER, &coeffs, sizeof(coeffs[0])*_nb_channels);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in eFuse write: %s", esp_err_to_name(err));
        return -1;
    }

    // Update coefficients in memory
    for (int j = 0; j < _nb_channels; j++) {
        _coeff_a[j] = a[j];
        _coeff_b[j] = b[j];
    }

    return 0;
}

void AnalogInputsHV::getAnalogCoeffs(float* as, float* bs)
{
    for (int i = 0; i < _nb_channels; i++) {
        as[i] = _coeff_a[i];
        bs[i] = _coeff_b[i];
    }
}

