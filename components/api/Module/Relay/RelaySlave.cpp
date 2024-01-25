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

#if defined(CONFIG_RELAY_HP) || defined(CONFIG_RELAY_LP)

void RelaySlave::init(void)
{
    RelayStandalone::init();
    ModuleSlave::init();

    onRequest(CMD_DIGITAL_WRITE, [](RequestMsg_t msg) -> uint32_t {
        RelayStandalone::digitalWrite((Relay_t)msg.param, (uint8_t)msg.data);
        return 0;
    });
}

#endif