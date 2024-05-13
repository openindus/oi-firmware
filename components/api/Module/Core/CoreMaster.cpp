/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CoreMaster.cpp
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CoreMaster.h"

#if defined(OI_CORE)

static const char CORE_TAG[] = "Core";

int CoreMaster::init(void)
{
    int err = ModuleMaster::init();
    err |= CoreStandalone::init();

    return err;
}

#endif
