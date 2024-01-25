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

#include "DiscretePinout.h"

#define DISCRETE_CONFIG_DOUT_DEFAULT()               \
{                                                       \
    .pin_bit_mask = (1ULL<<DISCRETE_PIN_DOUT_1) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_2) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_3) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_4) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_5) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_6) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_7) |    \
                    (1ULL<<DISCRETE_PIN_DOUT_8),     \
    .mode = GPIO_MODE_OUTPUT,                           \
    .pull_up_en = GPIO_PULLUP_DISABLE,                  \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,              \
    .intr_type = GPIO_INTR_DISABLE,                     \
}

#define DISCRETE_CONFIG_DIN_DEFAULT()               \
{                                                       \
    .pin_bit_mask = (1ULL<<DISCRETE_PIN_DIN_1) |    \
                    (1ULL<<DISCRETE_PIN_DIN_2) |    \
                    (1ULL<<DISCRETE_PIN_DIN_3) |    \
                    (1ULL<<DISCRETE_PIN_DIN_4) |    \
                    (1ULL<<DISCRETE_PIN_DIN_5) |    \
                    (1ULL<<DISCRETE_PIN_DIN_6) |    \
                    (1ULL<<DISCRETE_PIN_DIN_7) |    \
                    (1ULL<<DISCRETE_PIN_DIN_8) |    \
                    (1ULL<<DISCRETE_PIN_DIN_9) |    \
                    (1ULL<<DISCRETE_PIN_DIN_10),    \
    .mode = GPIO_MODE_INPUT,                            \
    .pull_up_en = GPIO_PULLUP_DISABLE,                  \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,              \
    .intr_type = GPIO_INTR_DISABLE,                     \
}
