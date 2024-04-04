/**
 * @file AnalogInput.cpp
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogInput.h"

static const char TAG[] = "AnalogInput";

bool AnalogInputAds866x::_deviceInitialized = false;

int AnalogInputAds866x::init(AnalogInputAds866xConfig_t* config)
{
    if (!_deviceInitialized) {
        ESP_LOGI(TAG, "Initialize device");

        if (config == NULL) {
            ESP_LOGE(TAG, "No configuration");
            return -1;
        } 

        _modePin = config->modePin;
        _mode = config->mode;

        if (_mode != AIN_MODE_VOLTAGE || _mode != AIN_MODE_CURRENT) {
            ESP_LOGE(TAG, "Invalid mode");
            return -1;
        } else {
            gpio_set_level(_modePin, _mode);
        }
        
        ads866x_init(config->ads866xConfig);
        setVoltageRange(_voltage_range);

        _deviceInitialized = true;
    }
    return 0;
}

int AnalogInputAds866x::read(void)
{
    return read(AIN_UNIT_RAW);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    float value = ads866x_analog_read(_num);
    float voltage = ads866x_convert_raw_2_volt(value, _voltage_range);

    switch (unit) {
        case AIN_UNIT_RAW:
            break;
        case AIN_UNIT_VOLTS:
            value = voltage;
            break;
        case AIN_UNIT_MILLIVOLTS:
            value = voltage / 1000.0f;
            break;

        case AIN_UNIT_MILLIAMPS:
            value = voltage / (1000.0f * AIN_CURRENT_MODE_RES_VALUE);
            break;

        case AIN_UNIT_AMPS:
            value = voltage / AIN_CURRENT_MODE_RES_VALUE;
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
            return -1;
    } else {
        _mode = mode;
        gpio_set_level(_modePin, _mode);
    }
}

uint8_t getMode(void)
{
    return (uint8_t)_mode;
}

void AnalogInputAds866x::setVoltageRange(AnalogInput_VoltageRange_t range)
{
    if (_mode == AIN_MODE_CURRENT) {
        if (range != AIN_VOLTAGE_RANGE_0_2V56) {
            ESP_LOGE(TAG,"Range in current mode cannot be modified !");
        } else {
            _voltage_range = range;
            ads866x_set_voltage_range(range);
        }
    }
    else if (range == AIN_VOLTAGE_RANGE_0_10V24 || range == AIN_VOLTAGE_RANGE_0_5V12
            || range == AIN_VOLTAGE_RANGE_0_2V56 || range == AIN_VOLTAGE_RANGE_0_1V28) {
        _voltage_range = range;
        ads866x_set_voltage_range(range);
    } else {
        ESP_LOGE(TAG, "Undefined range");
    }
}

uint8_t AnalogInputAds866x::getVoltageRange(void)
{
    return (uint8_t)_voltage_range;
}
