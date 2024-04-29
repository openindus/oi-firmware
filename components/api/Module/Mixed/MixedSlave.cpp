/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedSlave.cpp
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedSlave.h"

#if defined(OI_MIXED) && defined(MODULE_SLAVE)

int MixedSlave::init(void)
{
    int ret = 0;

    ret |= ModuleSlave::init();
    ret |= MixedStandalone::init();
    ret |= AnalogOutputsSlave::init();
    ret |= AnalogInputsLVSlave::init();
    ret |= DigitalOutputsSlave::init();
    ret |= DigitalInputsSlave::init();

    return ret;
}

#endif
