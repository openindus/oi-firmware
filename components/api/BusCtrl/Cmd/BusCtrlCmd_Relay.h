/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCtrlCmd_Relay.h
 * @author AFL
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_MASTER)

#include "BusCtrlCmd.h"
#include "Relays.h"

class BusCtrlCmd_Relay
{
public:

    BusCtrlCmd_Relay(BusCtrlCmd* control): _control(control) {}

    /**
     * @brief Set a relay at high or low level.
     * 
     * @param num Relay to drive.
     * @param level Relay level, HIGH or LOW.
     */
    void digitalWrite(Relay_Num_t num, uint8_t level);

    /**
     * @brief Toggle a Relay output
     * 
     * @param num Relay to toggle
    **/
    void toggleOutput(Relay_Num_t num);

private:

    BusCtrlCmd* _control;
};

#endif