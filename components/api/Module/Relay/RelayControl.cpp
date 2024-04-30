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

#if defined(MODULE_MASTER)

void RelayControl::digitalWrite(Relay_t relay, uint8_t level)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_WRITE, (uint8_t)relay, level};
    ctrlRequest(msgBytes);
}

#endif