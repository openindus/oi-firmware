/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedSlave.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "MixedStandalone.h"
#include "ModuleSlave.h"

#if defined(CONFIG_MIXED)

class MixedSlave : public MixedStandalone, public ModuleSlave
{
public:

    static void init(void);
    
private:

    static IsrCallback_t _isrCallback[4];
};

#endif