/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelayStandalone.h
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

typedef enum {
    RELAY_1 = 0,
    RELAY_2,
    RELAY_3,
    RELAY_4,
#if !defined(OI_RELAY_HP)
    RELAY_5,
    RELAY_6,
#endif
    RELAY_MAX
} Relay_t;

#if defined(OI_RELAY_HP) || defined(OI_RELAY_LP)

class RelayStandalone : public ModuleStandalone
{
public:

    static int init(void);

    /**
     * @brief Enable or disable relay
     * 
     */
    static void digitalWrite(Relay_t relay, uint8_t level);

private:

    static void _initRelays(void);

};

#endif