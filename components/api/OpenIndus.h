/**
 * @file OpenIndus.h
 * @brief OpenIndus API
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Module.h"
#ifndef OI_CORE_PLUS
#include "AnalogLS.h"
#include "Brushless.h"
#include "Core.h"
#include "Dc.h"
#include "Discrete.h"
#include "Mixed.h"
#include "RelayHP.h"
#include "Stepper.h"
#else
#include "CorePlus.h"
#endif

/* Arduino */
void setup(void);
void loop(void);
void error(int);

/* OI-Modules */
#ifndef OI_CORE_PLUS
#define OICore Core
#define OICoreLite Core
#define OIDiscrete Discrete
#define OIMixed Mixed
#define OIStepper Stepper
#define OIRelayHP RelayHP
#define OIAnalogLS AnalogLS
#define OIBrushless Brushless
#define OIDc Dc
#else
#define OICorePlus CorePlus
#endif