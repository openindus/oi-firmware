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

int AnalogInputAds866x::init(Ads866x_DeviceConfig_t* config)
{
    if (!_deviceInitialized) {
        ESP_LOGI(TAG, "Initialize device");

        if (config == NULL) {
            ESP_LOGE(TAG, "No configuration");
            return -1;
        } 

        Ads866x_DeviceConfig(config);
        Ads866x_Init();
    }
    return 0;
}

int AnalogInputAds866x::read(void)
{
    return Ads866x_AnalogRead(_num);
}

float AnalogInputAds866x::read(AnalogInput_Unit_t unit)
{
    float value = -1;
    switch (unit)
    {
    case AIN_UNIT_RAW:
        value = Ads866x_AnalogReadUnits(_num, ADS866x_UNITS_RAW);
        break;
    
    case AIN_UNIT_MILLIVOLTS:
        value = Ads866x_AnalogReadUnits(_num, ADS866x_UNITS_MILLIVOLTS);
        break;

    case AIN_UNIT_MILLIAMPS:
        value = Ads866x_AnalogReadUnits(_num, ADS866x_UNITS_MILLIAMPS);
        break;

    case AIN_UNIT_VOLTS:
        value = Ads866x_AnalogReadUnits(_num, ADS866x_UNITS_VOLTS);
        break;

    case AIN_UNIT_AMPS:
        value = Ads866x_AnalogReadUnits(_num, ADS866x_UNITS_AMPS);
        break;
    
    default:
        ESP_LOGE(TAG, "Undefined unit");
        break;
    }
    return value;
}

void AnalogInputAds866x::setMode(AnalogInput_Mode_t mode)
{
    if (mode == AIN_MODE_CURRENT) {
        Ads866x_setAdcMode(_num, ADS866X_CURRENT_MODE);
    } else if (mode == AIN_MODE_VOLTAGE) {
        Ads866x_setAdcMode(_num, ADS866X_VOLTAGE_MODE);
    } else {
        ESP_LOGE(TAG, "Undefined mode");
    }
}

void AnalogInputAds866x::setResolution(AnalogInput_Resolution_t res)
{
    if (res == AIN_RES_10_BITS) {
        Ads866x_setAdcResolution(ADS866x_ADC_RES_10BITS);
    } else if (res == AIN_RES_12_BITS) {
        Ads866x_setAdcResolution(ADS866x_ADC_RES_12BITS);
    } else {
        ESP_LOGE(TAG, "Undefined resolution");
    }    
}

void AnalogInputAds866x::setReference(float ref)
{
    Ads866x_setAnalogReference(ref);
}
