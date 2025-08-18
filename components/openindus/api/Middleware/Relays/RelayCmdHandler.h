/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Slave.h"
#include "Relays.h"

#if defined(CONFIG_MODULE_SLAVE)

class RelayCmdHandler
{
public:

    static int init();
};

#endif