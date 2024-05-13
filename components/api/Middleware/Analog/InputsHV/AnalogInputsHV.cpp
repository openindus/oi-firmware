/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHV.cpp
 * @brief
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */


#include "AnalogInputsHV.h"

static const char TAG[] = "AnalogInputsHV";

uint8_t AnalogInputsHV::_nb;
AnalogInputEsp32s3** AnalogInputsHV::_ains;

int AnalogInputsHV::init(const AdcNumChannel_t* channel, uint8_t nb) 
{
    ESP_LOGI(TAG, "Initialize Analog Inputs");

    _nb = nb;

    _ains = (AnalogInputEsp32s3**)calloc(_nb, sizeof(AnalogInputEsp32s3));

    for (size_t i = 0; i < nb; i++) {
        _ains[i] = new AnalogInputEsp32s3(i, channel[i]);
        _ains[i]->init();
    }

    return 0;
}

float AnalogInputsHV::read(AnalogInput_Num_t num, AnalogInput_Unit_t unit)
{
    if (num < _nb) {
        return _ains[num]->read(unit);
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
}

int AnalogInputsHV::analogRead(AnalogInput_Num_t num)
{
    if (num < _nb) {
        return _ains[num]->read();
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
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

    AnalogInput_eFuse_Coeff_t coeffs[_nb];

    for (int i = 0; i < _nb; i++) {

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
    esp_err_t err = esp_efuse_write_key(EFUSE_BLK_KEY1, ESP_EFUSE_KEY_PURPOSE_USER, &coeffs, sizeof(AnalogInput_eFuse_Coeff_t)*_nb);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error in eFuse write: %s", esp_err_to_name(err));
        return -1;
    }

    // Read coeffs
    for (int j = 0; j < _nb; j++) {
        _ains[j]->getCoeffs(NULL, NULL);
    }

    return 0;
}

void AnalogInputsHV::getAnalogCoeffs(float* as, float* bs)
{
    for (int i = 0; i < _nb; i++) {
        _ains[i]->getCoeffs(&as[i], &bs[i]);
    }
}

/******************* Analog Input Esp32s3 **********************/

AnalogInputEsp32s3::AnalogInputEsp32s3(uint8_t num, AdcNumChannel_t channel)
{
    _num = num;
    _channel = channel;
}

int AnalogInputEsp32s3::init()
{
    int err = 0;

    if (_channel.adc_num == ADC_UNIT_1) {
        err |= adc1_config_width(ADC_WIDTH_BIT_12);
        err |= adc1_config_channel_atten((adc1_channel_t)_channel.channel, ADC_ATTEN_DB_11);
        esp_adc_cal_characterize(_channel.adc_num, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &_adc_characteristic);
    }
    else if (_channel.adc_num == ADC_UNIT_2) {
        err |= adc1_config_channel_atten((adc1_channel_t)_channel.channel, ADC_ATTEN_DB_11);
        esp_adc_cal_characterize(_channel.adc_num, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &_adc_characteristic);
    } 
    else {
        ESP_LOGE(TAG, "Wrong ADC unit !");
    }

    getEFuseCoeffs();

    return err;
}

int AnalogInputEsp32s3::read(void)
{
    if (_channel.adc_num == ADC_UNIT_1) {
        return adc1_get_raw((adc1_channel_t)_channel.channel);
    } 
    else if (_channel.adc_num == ADC_UNIT_2) {
        int raw = -1;
        adc2_get_raw((adc2_channel_t)_channel.channel, ADC_WIDTH_12Bit, &raw);
        return raw;
    } 
    else {
        ESP_LOGE(TAG, "Wrong ADC unit !");
        return -1;
    }
}

float AnalogInputEsp32s3::read(AnalogInput_Unit_t unit)
{
    uint32_t voltage_sum = 0;

    // Making the mean with voltage and not adc_reading to be more precise
    for (int i = 0; i < ESP_ADC_NO_OF_SAMPLES; i++)
    {
        voltage_sum += esp_adc_cal_raw_to_voltage(read(), &_adc_characteristic);
    }

    // using float to increase precision
    float voltage = (float) voltage_sum / (float) ESP_ADC_NO_OF_SAMPLES;

    // Application coefficients
    float value = 0.0f;

    switch (unit) {
        case AIN_UNIT_RAW:
            value = voltage;
            break;
        case AIN_UNIT_VOLT:
            value = applyCoeffs(voltage)/1000.0f;
            break;
        case AIN_UNIT_MILLIVOLT:
            value = applyCoeffs(voltage);
            break;
        default:
            ESP_LOGE(TAG, "Undefined unit");
            break;
    }
    return value;
}

void AnalogInputEsp32s3::getEFuseCoeffs()
{
    if (esp_efuse_block_is_empty(EFUSE_BLK_KEY1) == false) {
        // getting coeff in eFuse
        esp_efuse_read_block(EFUSE_BLK_KEY1, &_coeff, sizeof(AnalogInput_eFuse_Coeff_t)*8*_num, sizeof(AnalogInput_eFuse_Coeff_t)*8);
        // Checking if coeff are valid
        if ((abs(_coeff.ain_coeff_a) < 100.0f) && (abs(_coeff.ain_coeff_b) < 1000.0f)) {
            ESP_LOGI(TAG, "Reading AIN_%i coeffs: a=%f b=%f", _num+1, _coeff.ain_coeff_a, _coeff.ain_coeff_b);
            return;
        }
    }    
    _coeff.ain_coeff_a = ESP_ADC_DEFAULT_COEFF_A;
    _coeff.ain_coeff_b = ESP_ADC_DEFAULT_COEFF_B;

    ESP_LOGI(TAG, "Reading default AIN_%i coeffs: a=%f b=%f", _num+1, _coeff.ain_coeff_a, _coeff.ain_coeff_b);
    
    return;
}

void AnalogInputEsp32s3::getCoeffs(float* a, float* b)
{
    getEFuseCoeffs();
    *a = _coeff.ain_coeff_a;
    *b = _coeff.ain_coeff_b;
}

float AnalogInputEsp32s3::applyCoeffs(float voltage)
{
    // Return 0mV is voltage is too low
    if (voltage < 0.1f) return voltage;
    return voltage * _coeff.ain_coeff_a  + _coeff.ain_coeff_b;
}
