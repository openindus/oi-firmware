/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelaySlave.h
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "RelayStandalone.h"
#include "ModuleSlave.h"

#if (defined(OI_RELAY_HP) || defined(OI_RELAY_LP)) && defined(MODULE_SLAVE)

class RelaySlave : public RelayStandalone, public ModuleSlave
{
public:

    static int init(void);

};

#endif
