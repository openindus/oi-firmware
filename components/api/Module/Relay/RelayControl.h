/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelayControl.h
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "RelayStandalone.h"

#if !defined(OI_RELAY_HP) & !defined(OI_RELAY_LP)

class RelayControl : public ModuleControl
{
public:

    RelayControl(int sn = 0) : ModuleControl(sn) {}

    void digitalWrite(Relay_t relay, uint8_t level);

};

#endif