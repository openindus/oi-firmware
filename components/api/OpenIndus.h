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

#include "Module.h"
#ifndef OI_CORE_PLUS
#include "Core.h"
#include "Discrete.h"
#include "Mixed.h"
#include "Stepper.h"
#include "RelayHP.h"
#include "AnalogLS.h"
#include "Brushless.h"
#include "Dc.h"
#else
#include "CorePlus.h"
#endif

void setup(void);
void loop(void);

#ifndef OI_CORE_PLUS
#define OICore      Core
#define OICoreLite  Core
#define OIDiscrete  Discrete
#define OIMixed     Mixed
#define OIStepper   Stepper
#define OIRelayHP   RelayHP
#define OIAnalogLS  AnalogLS
#define OIBrushless Brushless
#define OIDc        Dc
#else
#define OICorePlus  CorePlus
#endif