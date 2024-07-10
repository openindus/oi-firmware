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

enum Sensor_RTD_Type_e {
    TYPE_PT100 = (int) 0,
    TYPE_PT1000
};

enum Sensor_TC_Type_e {
    TYPE_K = (int) 0,
};

typedef int Sensor_Type_t;