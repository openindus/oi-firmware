/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OpenIndus.h
 * @brief OpenIndus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "System.h"

#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define OIMixed MixedStandalone
#elif defined(OI_MIXED) && !defined(MODULE_STANDALONE)
#define OIMixed MixedSlave
#else 
#define OIMixed MixedControl
#endif

#define OICore      Core
#define OIDiscrete  Discrete
#define OIStepper   Stepper
#define OIRelay     Relay
#define OIBrushless Brushless
#define OIAnalogls  Analogls
