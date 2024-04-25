/**
 * @file AnalogInputs.cpp
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInputs.h"

static const char TAG[] = "AnalogInputs";

AnalogInputs::AnalogInputs(const gpio_num_t* cmdGpio, uint8_t nb)
{
    for (size_t i = 0; i < nb; i++) {
        _ains[i] = new AnalogInputAds866x(i, cmdGpio[i]);
    }
    _nb = nb;
    _type = ANALOG_INPUT_ADS866X;
}

AnalogInputs::AnalogInputs(const adc1_channel_t* channel, uint8_t nb)
{
    for (size_t i = 0; i < nb; i++) {
        _ains_esp[i] = new AnalogInputEsp32s3(i, channel[i]);
    }
    _nb = nb;
    _type = ANALOG_INPUT_ESP32S3;
}

int AnalogInputs::init(ads866x_config_t *ads866xConfig, AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode) 
{
    ESP_LOGI(TAG, "Initialize Analog Inputs");

    if (_type == ANALOG_INPUT_ADS866X) {
        if (ads866xConfig == NULL) {
            ESP_LOGE(TAG, "No configuration");
            return -1;
        }

        ads866x_init(ads866xConfig);

        for (size_t i = 0; i < _nb; i++) {
            _ains[i]->init(range, mode);
        }
        return 0;
    } else {
        ESP_LOGE(TAG, "Function not available for this type of input");
    }
    return -1;
}

int AnalogInputs::init() 
{
    ESP_LOGI(TAG, "Initialize Analog Inputs");

    if (_type == ANALOG_INPUT_ESP32S3) {

        for (size_t i = 0; i < _nb; i++) {
            _ains_esp[i]->init();
        }
        return 0;
    } else {
        ESP_LOGE(TAG, "Function not available for this type of input");
    }
    return -1;
}

int AnalogInputs::read(AnalogInput_Num_t num)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            return _ains[num]->read();
        } 
        else if (_type == ANALOG_INPUT_ESP32S3) {
            return _ains_esp[num]->read();
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
}

float AnalogInputs::read(AnalogInput_Num_t num, AnalogInput_Unit_t unit)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            return _ains[num]->read(unit);
        } else if (_type == ANALOG_INPUT_ESP32S3) {
            return _ains_esp[num]->read(unit);
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
}

void AnalogInputs::setMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            _ains[num]->setMode(mode);
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }        
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
}

uint8_t AnalogInputs::getMode(AnalogInput_Num_t num)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            return _ains[num]->getMode();
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }   
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return 0;
}

void AnalogInputs::setVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            _ains[num]->setVoltageRange(range);
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }   
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
}

uint8_t AnalogInputs::getVoltageRange(AnalogInput_Num_t num)
{
    if (num < _nb) {
        if (_type == ANALOG_INPUT_ADS866X) {
            return _ains[num]->getVoltageRange();
        } else {
            ESP_LOGE(TAG, "Function not available for this type of input");
        }   
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return 0;
}

int AnalogInputs::setCoeffs(float* a, float* b)
{
    if (_type == ANALOG_INPUT_ESP32S3) {

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
                ESP_LOGE(TAG, "Invalid coefficients for AnalogInputs");
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
            _ains_esp[j]->getCoeffs();
        }

        return 0;
    } else {
        ESP_LOGE(TAG, "Function not available for this type of input");
    }   
    return -1;
}


/******************* Analog Input Ads866x **********************/

AnalogInputAds866x::AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio)
{
    if (num >= AIN_MAX) {
        ESP_LOGE(TAG, "Invalid Analog Input Number");
    } else {
        _num = num;
    }
    _modePin = cmdGpio;
}

void AnalogInputAds866x::init(AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode)
{
    gpio_config_t cfg;
        
    /* Configure Cmd pin */
    cfg.pin_bit_mask = (1ULL << _modePin);
    cfg.mode = GPIO_MODE_OUTPUT;
    cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&cfg));
    
    setMode(mode);
    setVoltageRange(range);
}

int AnalogInputAds866x::read(void)
{
    return ads866x_analog_read(_num);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    float value = ads866x_analog_read(_num);
    float voltage = ads866x_convert_raw_2_volt(value, _voltage_range);

    switch (unit) {
        case AIN_UNIT_RAW:
            break;
        case AIN_UNIT_VOLT:
            value = voltage;
            break;
        case AIN_UNIT_MILLIVOLT:
            value = voltage * 1000.0f;
            break;

        case AIN_UNIT_MILLIAMP:
            if (_mode != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "To read into MilliAmps, you should be into Current Mode");
                value = -1;
            } else {
                value = voltage * 1000 / AIN_CURRENT_MODE_RES_VALUE;
            }
            break;

        case AIN_UNIT_AMP:
        if (_mode != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "To read into Amps, you should be into Current Mode");
                value = -1;
            } else {
                value = voltage / AIN_CURRENT_MODE_RES_VALUE;
            }
            break;
        
        default:
            ESP_LOGE(TAG, "Undefined unit");
            break;
    }
    return value;
}

void AnalogInputAds866x::setMode(AnalogInput_Mode_t mode)
{
    if (mode != AIN_MODE_VOLTAGE && mode != AIN_MODE_CURRENT) {
            ESP_LOGE(TAG, "Invalid mode");
    } else {
        _mode = mode;
        gpio_set_level(_modePin, _mode);
    }
}

uint8_t AnalogInputAds866x::getMode(void)
{
    return (uint8_t)_mode;
}

void AnalogInputAds866x::setVoltageRange(AnalogInput_VoltageRange_t range)
{
    if (_mode == AIN_MODE_CURRENT) {
        if (range != AIN_VOLTAGE_RANGE_0_2V56) {
            ESP_LOGE(TAG,"Range in current mode should be 0-2V56 !");
        } else {
            _voltage_range = range;
            ads866x_set_channel_voltage_range(_num, _voltage_range);
        }
    }
    else if (_mode == AIN_MODE_VOLTAGE && (range == AIN_VOLTAGE_RANGE_0_10V24 || range == AIN_VOLTAGE_RANGE_0_5V12
            || range == AIN_VOLTAGE_RANGE_0_2V56 || range == AIN_VOLTAGE_RANGE_0_1V28)) {
        _voltage_range = range;
        ads866x_set_channel_voltage_range(_num, _voltage_range);
    } else {
        ESP_LOGE(TAG, "Undefined range or mode %d", _mode);
    }
}

uint8_t AnalogInputAds866x::getVoltageRange(void)
{
    return ads866x_get_channel_voltage_range(_num);
}

gpio_num_t AnalogInputAds866x::getModePin(void)
{
    return _modePin;
}

/******************* Analog Input Esp32s3 **********************/

AnalogInputEsp32s3::AnalogInputEsp32s3(uint8_t num, adc1_channel_t channel)
{
    _num = num;
    _channel = channel;
}

void AnalogInputEsp32s3::init()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(_channel, ADC_ATTEN_DB_11);
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &_adc_characteristic);
    getCoeffs();
}

int AnalogInputEsp32s3::read(void)
{
    return adc1_get_raw(_channel);
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

void AnalogInputEsp32s3::getCoeffs(void)
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

float AnalogInputEsp32s3::applyCoeffs(float voltage)
{
    // Return 0mV is voltage is too low
    if (voltage < 0.1f) return voltage;
    return voltage * _coeff.ain_coeff_a  + _coeff.ain_coeff_b;
}
