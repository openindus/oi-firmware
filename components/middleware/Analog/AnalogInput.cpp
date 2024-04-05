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

gpio_num_t AnalogInputAds866x::_modePin[AIN_MAX] = {
    GPIO_NUM_NC,
    GPIO_NUM_NC,
    GPIO_NUM_NC,
    GPIO_NUM_NC
};

AnalogInput_Mode_t AnalogInputAds866x::_mode[AIN_MAX] = {
    AIN_MODE_UNDEFINED,
    AIN_MODE_UNDEFINED,
    AIN_MODE_UNDEFINED,
    AIN_MODE_UNDEFINED
};

AnalogInput_VoltageRange_t AnalogInput::_voltage_range[AIN_MAX] = {
    AIN_VOLTAGE_RANGE_UNDEFINED,
    AIN_VOLTAGE_RANGE_UNDEFINED,
    AIN_VOLTAGE_RANGE_UNDEFINED,
    AIN_VOLTAGE_RANGE_UNDEFINED
};

int AnalogInputAds866x::init(AnalogInputAds866xConfig_t* config)
{
    if (!_deviceInitialized) {
        ESP_LOGI(TAG, "Initialize device");

        if (config == NULL) {
            ESP_LOGE(TAG, "No configuration");
            return -1;
        }

        for (size_t i = 0; i < AIN_MAX; i++) {
            _mode[i] = config->mode[i];
            _modePin[i] = config->modePin[i];

            if (_mode[i] != AIN_MODE_VOLTAGE || _mode[i] != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "Invalid mode");
                return -1;
            } else {
                gpio_set_level(_modePin[i], _mode[i]);
            }
        }
        
        ads866x_init(config->ads866xConfig);

        for (size_t i = 0; i < AIN_MAX; i++) {
            setVoltageRange(_voltage_range[i]);
        }

        _deviceInitialized = true;
    }
    return 0;
}

int AnalogInputAds866x::read(void)
{
    return ads866x_analog_read(_num);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    uint16_t value = ads866x_analog_read(_num);
    float voltage = ads866x_convert_raw_2_volt(value, _voltage_range[_num]);

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
            if (_mode[_num] != AIN_MODE_CURRENT) {
                ESP_LOGE(TAG, "To read into MilliAmps, you should be into Current Mode");
                value = -1;
            } else {
                value = voltage / (1000.0f * AIN_CURRENT_MODE_RES_VALUE);
            }
            break;

        case AIN_UNIT_AMP:
        if (_mode[_num] != AIN_MODE_CURRENT) {
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
    if (_mode[_num] != AIN_MODE_VOLTAGE || _mode[_num] != AIN_MODE_CURRENT) {
            ESP_LOGE(TAG, "Invalid mode");
    } else {
        _mode[_num] = mode;
        gpio_set_level(_modePin[_num], _mode[_num]);
    }
}

uint8_t AnalogInputAds866x::getMode(void)
{
    return (uint8_t)_mode[_num];
}

void AnalogInputAds866x::setVoltageRange(AnalogInput_VoltageRange_t range)
{
    if (_mode[_num] == AIN_MODE_CURRENT) {
        if (range != AIN_VOLTAGE_RANGE_0_2V56) {
            ESP_LOGE(TAG,"Range in current mode should be 0-2V56 !");
        } else {
            _voltage_range[_num] = range;
            ads866x_set_channel_voltage_range(_num, _voltage_range[_num]);
        }
    }
    else if (range == AIN_VOLTAGE_RANGE_0_10V24 || range == AIN_VOLTAGE_RANGE_0_5V12
            || range == AIN_VOLTAGE_RANGE_0_2V56 || range == AIN_VOLTAGE_RANGE_0_1V28) {
        _voltage_range[_num] = range;
        ads866x_set_channel_voltage_range(_num, _voltage_range[_num]);
    } else {
        ESP_LOGE(TAG, "Undefined range");
    }
}

uint8_t AnalogInputAds866x::getVoltageRange(void)
{
    return (uint8_t)_voltage_range[_num];
}
