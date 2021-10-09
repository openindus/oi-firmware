/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OpenIndus.h.h
 * @brief include OpenIndus 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "OISystem.h"
#include "OIConsole.h"
#include "OIType.h"

#include "OICore.h"
#include "OIDiscrete.h"
#include "OIStepper.h"
#include "OIMixed.h"
#include "OIRelay.h"

#include "OICAN.h"
#include "OIRS.h"

#if defined CONFIG_OI_CORE
extern OICore Core;
#define Module Core
#elif (defined CONFIG_OI_DISCRETE) || (defined CONFIG_OI_DISCRETE_VERTICAL)
extern OIDiscrete Discrete;
#define Module Discrete
#elif defined CONFIG_OI_STEPPER
extern OIStepper Stepper;
#define Module Stepper
#elif defined CONFIG_OI_MIXED
extern OIMixed Mixed;
#define Module Mixed
#elif (defined CONFIG_OI_RELAY_HP) || (defined CONFIG_OI_RELAY_LP)
extern OIRelay Relay;
#define Module Relay
#endif

void setup(void);
void loop(void);