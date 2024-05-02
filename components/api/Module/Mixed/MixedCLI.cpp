/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedCLI.cpp
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedCLI.h"

#if defined(OI_MIXED)

int MixedCLI::init(void)
{
    int err = 0;

    err |= AnalogOutputsCLI::init();
    err |= AnalogInputsLVCLI::init();
    err |= DigitalOutputsCLI::init();
    err |= DigitalInputsCLI::init();

    return err;
}

#endif
