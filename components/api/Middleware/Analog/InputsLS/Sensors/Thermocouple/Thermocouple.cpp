/**
 * @file Thermocouple.cpp
 * @brief Thermocouple
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Thermocouple.h"

static const char TAG[] = "Thermocouple";

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

    return voltage;
}

/**
 * @brief Read temperature (°C)
 * 
 * @param timeMs Conversion time in milliseconds
 * @return float Temperature value
 */
float Thermocouple::readTemperature(uint32_t timeMs)
{
    return 0.0;
}