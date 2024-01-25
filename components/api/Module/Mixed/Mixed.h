/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Mixed.h
 * @brief Functions for Mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "MixedStandalone.h"
#include "MixedSlave.h"
#include "MixedControl.h"

#if defined(CONFIG_MIXED)

#if defined(CONFIG_MODULE_STANDALONE)
class Mixed : public MixedStandalone {};
#else 
class Mixed : public MixedSlave {};
#endif

#else
class Mixed : public MixedControl {};

#endif