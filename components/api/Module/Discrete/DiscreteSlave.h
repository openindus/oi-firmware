/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteSlave.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "DiscreteStandalone.h"
#include "ModuleSlave.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

class DiscreteSlave : public ModuleSlave
{
public:

    static int init(void);
    
private:

    static DiscreteStandalone* _discrete;
    static IsrCallback_t _isrCallback[4];
};

#endif