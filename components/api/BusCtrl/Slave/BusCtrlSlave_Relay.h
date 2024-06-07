/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "BusCtrlSlave.h"
#include "Relays.h"

class BusCtrlSlave_Relay
{
public:

    static int init();
};

#endif