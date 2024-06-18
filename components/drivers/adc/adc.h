/**
 * @file adc.h
 * @brief ADC
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "driver/adc.h"

typedef struct {
    adc_unit_t unit;
    adc_channel_t channel;
} adc_num_t;
