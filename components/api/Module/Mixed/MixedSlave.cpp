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
    int err = 0;

    err |= ModuleSlave::init();
    err |= MixedStandalone::init();
    err |= AnalogOutputsSlave::init();
    err |= AnalogInputsLVSlave::init();
    err |= DigitalOutputsSlave::init();
    err |= DigitalInputsSlave::init();

    return err;
}

#endif
