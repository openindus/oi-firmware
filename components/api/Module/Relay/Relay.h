/**
 * @file Relay.h
 * @brief Relay
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Relay.h"
#include "Module.h"

typedef enum {
    RELAY_1 = DOUT_1,
    RELAY_2 = DOUT_2,
    RELAY_3 = DOUT_3,
    RELAY_4 = DOUT_4,
    RELAY_5 = DOUT_5,
    RELAY_6 = DOUT_6,
    RELAY_MAX
} Relay_Num_t;

class Relay
{
public:

    static inline int digitalWrite(Relay_Num_t num, uint8_t level) {
        return -1;
    };
};