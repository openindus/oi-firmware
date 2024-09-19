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
    RTD_TWO_WIRE,
    RTD_THREE_WIRE,
    THERMOCOUPLE_J, // From -210°C to 1200°C
    THERMOCOUPLE_K, // From -270°C to 1370°C
    THERMOCOUPLE_T, // From -200°C to 400°C
    THERMOCOUPLE_E, // From -270°C to 1000°C
    THERMOCOUPLE_S, // From -50°C to 1768°C
    STRAIN_GAUGE,
};
