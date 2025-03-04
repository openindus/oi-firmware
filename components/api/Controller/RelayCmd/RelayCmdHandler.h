/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "ControllerTypes.h"
#include "Slave.h"
#include "Relays.h"

class RelayCmdHandler
{
public:

    static int init();
};

#endif