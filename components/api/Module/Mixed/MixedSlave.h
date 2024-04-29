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

#if defined(OI_MIXED) && defined(MODULE_SLAVE)

class MixedSlave : public ModuleSlave, public MixedStandalone
{
public:

    static int init(void);
    
private:

    static MixedStandalone* _mixed;
    static IsrCallback_t _isrCallback[4];
};

#endif