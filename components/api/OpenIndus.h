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
#include "Core.h"
#ifndef OI_CORE_PLUS
#include "Discrete.h"
#include "Mixed.h"
#include "Stepper.h"
#include "RelayHP.h"
#include "AnalogLS.h"
#include "Brushless.h"
#include "Dc.h"
#endif

void setup(void);
void loop(void);

#define OICore      Core

#ifndef OI_CORE_PLUS
#define OIDiscrete  Discrete
#define OIMixed     Mixed
#define OIStepper   Stepper
#define OIRelayHP   RelayHP
#define OIAnalogLS  AnalogLS
#define OIBrushless Brushless
#define OIDc        Dc
#endif