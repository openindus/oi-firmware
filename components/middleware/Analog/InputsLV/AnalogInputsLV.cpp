/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLV.h
 * @brief
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */


#include "AnalogInputsLV.h"

static const char TAG[] = "AnalogInputsLV";

uint8_t AnalogInputsLV::_nb;
AnalogInputAds866x** AnalogInputsLV::_ains;
uint8_t* AnalogInputsLV::_current_sat;

int AnalogInputsLV::init(ads866x_config_t *ads866xConfig, const ain_num_t* num, const gpio_num_t* cmdGpio, uint8_t nb) 
{
    ESP_LOGI(TAG, "Initialize Analog Inputs");
    
    int err = 0;
    _nb = nb;

    if (ads866xConfig == NULL) {
        ESP_LOGE(TAG, "No configuration");
        return -1;
    }

    /* Init driver */
    err |= ads866x_init(ads866xConfig);

    _ains = (AnalogInputAds866x**)calloc(_nb, sizeof(AnalogInputAds866x));
    _current_sat = (uint8_t*)calloc(_nb, sizeof(uint8_t));

    for (size_t i = 0; i < _nb; i++) {
        _ains[i] = new AnalogInputAds866x(num[i], cmdGpio[i]);
        err |= _ains[i]->init(AIN_DEFAULT_RANGE, AIN_DEFAULT_MODE);
        _current_sat[i] = 0;
    }

    /* Create control task for overcurrent */
    ESP_LOGI(TAG, "Create AIN control task");
    xTaskCreate(_controlTask, "Control ain task", 4096, NULL, 1, NULL);

    _registerCLI();
    
    return err;

}

float AnalogInputsLV::read(AnalogInput_Num_t num, AnalogInput_Unit_t unit)
{
    if (num < _nb) {
        return _ains[num]->read(unit);
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
}

int AnalogInputsLV::analogRead(AnalogInput_Num_t num)
{
    if (num < _nb) {
        return _ains[num]->read();
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return -1;
}

float AnalogInputsLV::analogReadVolt(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_VOLT);
}

float AnalogInputsLV::analogReadMilliVolt(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_MILLIVOLT);
}

float AnalogInputsLV::analogReadAmp(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_AMP);
}

float AnalogInputsLV::analogReadMilliAmp(AnalogInput_Num_t num)
{
    return read(num, AIN_UNIT_MILLIAMP);
}

void AnalogInputsLV::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    if (num < _nb) {
         _ains[num]->setMode(mode);
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
}

uint8_t AnalogInputsLV::analogInputGetMode(AnalogInput_Num_t num)
{
    if (num < _nb) {
        return _ains[num]->getMode();
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return 0;
}

void AnalogInputsLV::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    if (num < _nb) {
        _ains[num]->setVoltageRange(range);
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
}

uint8_t AnalogInputsLV::analogInputGetVoltageRange(AnalogInput_Num_t num)
{
    if (num < _nb) {
        return _ains[num]->getVoltageRange();
    } else {
        ESP_LOGE(TAG, "INVALID INPUT AIN_%i", num+1);
    }
    return 0;
}

/* Every 500ms check if there is a current saturation on a channel
 * and switch to voltage mode if it lasts more than 30s */
void AnalogInputsLV::_controlTask(void *pvParameters)
{
    while (1) {
        for (size_t i = 0; i < _nb; i++) {
            if (_ains[i]->getMode() == AIN_MODE_CURRENT) {
                if (_ains[i]->read(AIN_UNIT_MILLIAMP) > AIN_SAT_CURRENT_AMP) {
                    _current_sat[i] += 1;
                    if (_current_sat[i] >= 60) {
                        _ains[i]->setMode(AIN_MODE_VOLTAGE);
                        _current_sat[i] = 0;
                        ESP_LOGE(TAG, "Overcurrent for more than 30s on AIN_%i, switching to voltage mode", i+1);
                    }
                }
                else {
                    _current_sat[i] = 0;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/******************* Analog Input Ads866x **********************/

AnalogInputAds866x::AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio)
{
    if (num >= 4) {
        ESP_LOGE(TAG, "Invalid Analog Input Number");
    } else {
        _num = num;
    }
    _modePin = cmdGpio;
}

int AnalogInputAds866x::init(AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode)
{
    int err = 0;
    gpio_config_t cfg;
        
    /* Configure Cmd pin */
    cfg.pin_bit_mask = (1ULL << _modePin);
    cfg.mode = GPIO_MODE_OUTPUT;
    cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;

    err |= gpio_config(&cfg);
    
    _mutex = xSemaphoreCreateMutex();
    xSemaphoreGive(_mutex);

    setMode(mode);
    setVoltageRange(range);

    return err;
}

int AnalogInputAds866x::read(void)
{
    return ads866x_analog_read(_num);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    if ((unit == AIN_UNIT_MILLIAMP || unit == AIN_UNIT_AMP) && _mode != AIN_MODE_CURRENT) {
        ESP_LOGE(TAG, "To read into MilliAmps or Amps, you should be into Current Mode");
        return -1;
    }

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
            value = voltage * 1000 / AIN_CURRENT_MODE_RES_VALUE;
            break;

        case AIN_UNIT_AMP:
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
    if (mode != AIN_MODE_VOLTAGE && mode != AIN_MODE_CURRENT) {
            ESP_LOGE(TAG, "Invalid mode");
    } else {
        xSemaphoreTake(_mutex, portMAX_DELAY);
        _mode = mode;
        gpio_set_level(_modePin, _mode);
        xSemaphoreGive(_mutex);

        if (_mode == AIN_MODE_CURRENT) {
            setVoltageRange(AIN_VOLTAGE_RANGE_0_2V56);
        }
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