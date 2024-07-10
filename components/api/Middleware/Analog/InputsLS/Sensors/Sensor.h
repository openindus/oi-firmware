/**
 * @file Sensor.h
 * @brief Sensor
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

typedef enum {
    RTD_TWO_WIRE = 0,
    RTD_THREE_WIRE,
    THERMOCOUPLE,
    STRAIN_GAUGE,
} Sensor_t;

typedef int Sensor_Type_t;