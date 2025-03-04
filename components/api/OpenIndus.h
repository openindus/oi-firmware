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

/* Digital intputs num */
#define DIN1    (DIn_Num_t) 0x01
#define DIN2    (DIn_Num_t) 0x02
#define DIN3    (DIn_Num_t) 0x03
#define DIN4    (DIn_Num_t) 0x04
#define DIN5    (DIn_Num_t) 0x05
#define DIN6    (DIn_Num_t) 0x06
#define DIN7    (DIn_Num_t) 0x07
#define DIN8    (DIn_Num_t) 0x08
#define DIN9    (DIn_Num_t) 0x09
#define DIN10   (DIn_Num_t) 0x0A

/* Digital outtputs num */
#define DOUT1   (DOut_Num_t) 0x01
#define DOUT2   (DOut_Num_t) 0x02
#define DOUT3   (DOut_Num_t) 0x03
#define DOUT4   (DOut_Num_t) 0x04
#define DOUT5   (DOut_Num_t) 0x05
#define DOUT6   (DOut_Num_t) 0x06
#define DOUT7   (DOut_Num_t) 0x07
#define DOUT8   (DOut_Num_t) 0x08
#define DOUT9   (DOut_Num_t) 0x09
#define DOUT10  (DOut_Num_t) 0x0A