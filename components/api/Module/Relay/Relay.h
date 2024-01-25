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

#if defined(CONFIG_RELAY_HP) || defined(CONFIG_RELAY_LP)

#if defined(CONFIG_MODULE_STANDALONE)
class Relay : public RelayStandalone {};
#else 
class Relay : public RelaySlave {};
#endif

#else
class Relay : public RelayControl {};

#endif