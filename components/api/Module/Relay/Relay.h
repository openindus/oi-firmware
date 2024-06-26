/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Relay.h
 * @brief Functions for Relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "RelayStandalone.h"
#include "RelaySlave.h"
#include "RelayControl.h"

#if (defined(OI_RELAY_HP) || defined(OI_RELAY_LP)) && defined(MODULE_STANDALONE)
#define Relay RelayStandalone
#elif (defined(OI_RELAY_HP) || defined(OI_RELAY_LP)) && !defined(MODULE_STANDALONE)
#define Relay RelaySlave
#else 
#define Relay RelayControl
#endif
