/**
 * @file BusControl.h
 * @brief BusControl interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

typedef enum {
    STATE_UNDEFINED = (int) -1,
    STATE_IDLE      = (int) 0,
    STATE_RUNNING   = (int) 1,
    STATE_ERROR     = (int) 2
} BusControl_State_t;