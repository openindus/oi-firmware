/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInput.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ads866x/ads866x.h"

typedef enum {
    AIN_1 = 0,
    AIN_2,
#if !defined(CONFIG_DISCRETE) && !defined(CONFIG_DISCRETE_VE)
    AIN_3,
    AIN_4,
#if !defined(CONFIG_MIXED)
    AIN_5,
    AIN_6,
    AIN_7,
    AIN_8,
    AIN_9,
    AIN_10,
#endif
#endif
    AIN_MAX
} AnalogInputNum_t;

typedef enum {
    ADC_MODE_CURRENT = ADS866X_CURRENT_MODE,
    ADC_MODE_VOLTAGE = ADS866X_VOLTAGE_MODE
} AdcMode_t;

typedef enum
{
    ADC_RES_10_BITS = ADS866x_ADC_RES_10BITS,
    ADC_RES_12_BITS = ADS866x_ADC_RES_12BITS
} AdcResBits_t;

