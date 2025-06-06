/**
 * @file OpenIndus.h
 * @brief OpenIndus API
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "System.h"
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

/* OpenIndus modules */
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

/* Arduino RS485 */
class RS485Class : public RS {};

/* Digital Inputs */
#define DIN1    DIN_1
#define DIN2    DIN_2
#define DIN3    DIN_3
#define DIN4    DIN_4
#define DIN5    DIN_5
#define DIN6    DIN_6
#define DIN7    DIN_7
#define DIN8    DIN_8
#define DIN9    DIN_9
#define DIN10   DIN_10

/* Digital Outputs */
#define DOUT1   DOUT_1
#define DOUT2   DOUT_2
#define DOUT3   DOUT_3
#define DOUT4   DOUT_4
#define DOUT5   DOUT_5
#define DOUT6   DOUT_6
#define DOUT7   DOUT_7
#define DOUT8   DOUT_8
#define DOUT9   DOUT_9
#define DOUT10  DOUT_10