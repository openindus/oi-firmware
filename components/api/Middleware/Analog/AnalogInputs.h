/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputs.h
 * @brief
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "driver/adc.h"

typedef enum {
    AIN_1 = 0,
    AIN_2 = 1,
    AIN_3 = 2,
    AIN_4 = 3,
    AIN_MAX
} AnalogInput_Num_t;

typedef enum {
    AIN_MODE_VOLTAGE = 0,
    AIN_MODE_CURRENT = 1,
    AIN_MODE_UNDEFINED
} AnalogInput_Mode_t;

typedef enum {
    AIN_VOLTAGE_RANGE_0_10V24 = 5,
    AIN_VOLTAGE_RANGE_0_5V12 = 6,
    AIN_VOLTAGE_RANGE_0_2V56 = 7,
    AIN_VOLTAGE_RANGE_0_1V28 = 8,
    AIN_VOLTAGE_RANGE_UNDEFINED
} AnalogInput_VoltageRange_t;

typedef enum {
    AIN_UNIT_RAW = 0,
    AIN_UNIT_MILLIVOLT = 1,
    AIN_UNIT_MILLIAMP = 2,
    AIN_UNIT_VOLT = 3,
    AIN_UNIT_AMP = 4,
    AIN_UNIT_UNDEFINED
} AnalogInput_Unit_t;

typedef struct {
    adc_unit_t adc_num;
    adc_channel_t channel;
} AdcNumChannel_t;