/**
 * @file Sensors.h
 * @brief Sensors
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

typedef enum {
    RTD_2_WIRE = 0,
    RTD_3_WIRE,
    THERMOCOUPLE,
    STRAIN_GAUGE,
} Sensors_Type_t;