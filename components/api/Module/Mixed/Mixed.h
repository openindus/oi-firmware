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
#include "MixedCLI.h"

#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define Mixed MixedStandalone
#elif defined(OI_MIXED) && !defined(MODULE_STANDALONE)
#define Mixed MixedSlave
#else 
#define Mixed MixedControl
#endif
