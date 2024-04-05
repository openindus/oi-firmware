/**
 * @file AnalogInput.cpp
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInput.h"

static const char TAG[] = "AnalogInputs";

AnalogInputs::AnalogInputs(const gpio_num_t* cmdGpio, uint8_t nb, ads866x_config_t *ads866xConfig, AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode)
{
    for (size_t i = 0; i < nb; i++) {
        _ains[i] = new AnalogInputAds866x(i, cmdGpio[i], range, mode);
    }
    _nb = nb;
    _type = ANALOG_INPUT_ADS866X;
    _ads866xConfig = ads866xConfig;
}

int AnalogInputs::init(void) {
    ESP_LOGI(TAG, "Initialize Analog Inputs");

    if (_type == ANALOG_INPUT_ADS866X) {
        if (_ads866xConfig == NULL) {
            ESP_LOGE(TAG, "No configuration");
            return -1;
        }

        /** Set mosfet state according to the chosen mode */
        for (size_t i = 0; i < _nb; i++)
        {        
            if (_ains[i]->getMode() != AIN_MODE_VOLTAGE || _ains[i]->getMode() != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "Invalid mode");
                return -1;
            } else {
                gpio_set_level(_ains[i]->getModePin(), _ains[i]->getMode());
            }
        }

        ads866x_init(_ads866xConfig);

        for (size_t i = 0; i < _nb; i++) {
            _ains[i]->setVoltageRange((AnalogInput_VoltageRange_t)_ains[i]->getVoltageRange());
        }
        return 0;
    }
    return -1;
}

int AnalogInputs::read(AnalogInput_Num_t num)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        return _ains[num]->read();
    }
    return -1;
}

float AnalogInputs::read(AnalogInput_Num_t num, AnalogInput_Unit_t unit)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        return _ains[num]->read(unit);
    }
    return -1;
}

void AnalogInputs::setMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        _ains[num]->setMode(mode);
    }
}

uint8_t AnalogInputs::getMode(AnalogInput_Num_t num)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        return _ains[num]->getMode();
    }
    return 0;
}

void AnalogInputs::setVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        _ains[num]->setVoltageRange(range);
    }
}

uint8_t AnalogInputs::getVoltageRange(AnalogInput_Num_t num)
{
    if (_type == ANALOG_INPUT_ADS866X) {
        return _ains[num]->getVoltageRange();
    }
    return 0;
}


/******************* Analog Input Ads866x **********************/

AnalogInputAds866x::AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio, AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode)
{
    if (num >= AIN_MAX) {
        ESP_LOGE(TAG, "Invalid Analog Input Number");
    } else {
        _num = num;
    }
    if (mode != AIN_MODE_VOLTAGE || mode != AIN_MODE_CURRENT) {
        ESP_LOGE(TAG, "Invalid mode");
    } else {
        _mode = mode;
    }
    if (range >= AIN_VOLTAGE_RANGE_UNDEFINED) {
        ESP_LOGE(TAG, "Invalid Voltage Range");
    } else {
        _voltage_range = range;
    }
    _modePin = cmdGpio;
}


int AnalogInputAds866x::read(void)
{
    return ads866x_analog_read(_num);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    uint16_t value = ads866x_analog_read(_num);
    float voltage = ads866x_convert_raw_2_volt(value, _voltage_range);

    switch (unit) {
        case AIN_UNIT_RAW:
            ESP_LOGE(TAG, "To read into Raw unit, use the read() function");    //TODO: à tester
            break;
        case AIN_UNIT_VOLT:
            value = voltage;
            break;
        case AIN_UNIT_MILLIVOLT:
            value = voltage / 1000.0f;
            break;

        case AIN_UNIT_MILLIAMP:
            if (_mode != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "To read into MilliAmps, you should be into Current Mode");
                value = -1;
            } else {
                value = voltage / (1000.0f * AIN_CURRENT_MODE_RES_VALUE);
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
    if (_mode != AIN_MODE_VOLTAGE || _mode != AIN_MODE_CURRENT) {
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
    else if (range == AIN_VOLTAGE_RANGE_0_10V24 || range == AIN_VOLTAGE_RANGE_0_5V12
            || range == AIN_VOLTAGE_RANGE_0_2V56 || range == AIN_VOLTAGE_RANGE_0_1V28) {
        _voltage_range = range;
        ads866x_set_channel_voltage_range(_num, _voltage_range);
    } else {
        ESP_LOGE(TAG, "Undefined range");
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
