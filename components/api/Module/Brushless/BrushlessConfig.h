/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteStandalone.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "BrushlessPinout.h"

#define BRUSHLESS_CONFIG_DOUT_DEFAULT()               \
{                                                     \
    .pin_bit_mask = (1ULL<<BRUSHLESS_PIN_DOUT_1) |    \
                    (1ULL<<BRUSHLESS_PIN_DOUT_2),     \
    .mode = GPIO_MODE_OUTPUT,                         \
    .pull_up_en = GPIO_PULLUP_DISABLE,                \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,            \
    .intr_type = GPIO_INTR_DISABLE,                   \
}

#define BRUSHLESS_CONFIG_DIN_DEFAULT()               \
{                                                     \
    .pin_bit_mask = (1ULL<<BRUSHLESS_PIN_DIN_1) |    \
                    (1ULL<<BRUSHLESS_PIN_DIN_2),     \
    .mode = GPIO_MODE_INPUT,                          \
    .pull_up_en = GPIO_PULLUP_DISABLE,                \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,            \
    .intr_type = GPIO_INTR_DISABLE,                   \
}
