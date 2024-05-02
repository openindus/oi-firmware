/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteCLI::h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteCLI.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

int DiscreteCLI::init(void)
{
    int err = 0;

    err |= AnalogInputsHVCLI::init();
    err |= DigitalOutputsCLI::init();
    err |= DigitalInputsCLI::init();

    return err;
}

#endif
