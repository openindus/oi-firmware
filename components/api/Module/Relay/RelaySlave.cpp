/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelaySlave.cpp
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "RelaySlave.h"

#if defined(OI_RELAY_HP) || defined(OI_RELAY_LP)

void RelaySlave::init(void)
{
    ModuleSlave::init();

    addCtrlCallback(CONTROL_DIGITAL_WRITE, [](std::vector<uint8_t>& data) { 
        RelayStandalone::digitalWrite((Relay_t)data[1], data[2]);
    });
}

#endif