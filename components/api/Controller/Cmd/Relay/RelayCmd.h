/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */


#pragma once

#if defined(MODULE_MASTER)

#include "Slave.h"
#include "Master.h"
#include "Relays.h"

class RelayCmd
{
public:

    RelayCmd(Controller* control): _controller(control) {}

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

    Controller* _controller;
};

#endif