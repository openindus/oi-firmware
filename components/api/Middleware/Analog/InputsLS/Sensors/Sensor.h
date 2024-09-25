/**
 * @file Sensor.h
 * @brief Sensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

enum Sensor_Type_e {
    RAW_SENSOR, // Read differential input voltage
    RTD_PT100,
    RTD_PT1000,
    THERMOCOUPLE_B, // From 250°C to 1820°C
    THERMOCOUPLE_E, // From -200°C to 1000°C 
    THERMOCOUPLE_J, // From -210°C to 1200°C
    THERMOCOUPLE_K, // From -270°C to 1372°C
    THERMOCOUPLE_N, // From -200°C to 1200°C
    THERMOCOUPLE_R, // From -50°C to 1768°C
    THERMOCOUPLE_S, // From -50°C to 1768°C
    THERMOCOUPLE_T, // From -200°C to 400°C
    STRAIN_GAUGE,
};


enum Sensor_Gain_e {
    GAIN_1 = 0,
    GAIN_2,
    GAIN_4,
    GAIN_8,
    GAIN_16,
    GAIN_32,
    GAIN_64,
    GAIN_128
};
