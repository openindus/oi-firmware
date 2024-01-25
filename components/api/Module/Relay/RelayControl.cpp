/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelayControl.cpp
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "RelayControl.h"

#if !defined(CONFIG_RELAY_HP) & !defined(CONFIG_RELAY_LP)

void RelayControl::digitalWrite(Relay_t relay, uint8_t level)
{
    RequestMsg_t msg;
    msg.cmd = CMD_DIGITAL_WRITE;
    msg.param = (uint16_t)relay;
    msg.data = (uint32_t)level;
    request(msg);
}

#endif