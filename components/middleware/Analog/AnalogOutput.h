/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogOutput.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "dac8760/dac8760.h"

typedef enum {
    AOUT_1 = 0,
    AOUT_2,
} AnalogOutputNum_t;

typedef enum
{
    DAC_VOLTAGE_RANGE_0_5_V = DAC8760_VOLTAGE_RANGE_0_5_V,
    DAC_VOLTAGE_RANGE_0_10_V = DAC8760_VOLTAGE_RANGE_0_10_V,
    DAC_VOLTAGE_RANGE_N5_5_V = DAC8760_VOLTAGE_RANGE_N5_5_V,
    DAC_VOLTAGE_RANGE_N10_10_V = DAC8760_VOLTAGE_RANGE_N10_10_V
} DacVoltageMode_t;

typedef enum
{
    DAC_CURRENT_RANGE_4_20_MA = DAC8760_CURRENT_RANGE_4_20_MA,
    DAC_CURRENT_RANGE_0_20_MA = DAC8760_CURRENT_RANGE_0_20_MA,
    DAC_CURRENT_RANGE_0_24_MA = DAC8760_CURRENT_RANGE_0_24_MA
} DacCurrentMode_t;
