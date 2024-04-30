/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteSlave.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteSlave.h"

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)

int DiscreteSlave::init(void)
{
    int ret = 0;

    ret |= ModuleSlave::init();
    ret |= DiscreteStandalone::init();
    ret |= AnalogInputsHVSlave::init();
    ret |= DigitalOutputsSlave::init();
    ret |= DigitalInputsSlave::init();

    return ret;
}

#endif