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

/* Digital outputs num */
#define DOUT1 DOut_Num_t::DOUT1
#define DOUT2 DOut_Num_t::DOUT2
#define DOUT3 DOut_Num_t::DOUT3
#define DOUT4 DOut_Num_t::DOUT4
#define DOUT5 DOut_Num_t::DOUT5
#define DOUT6 DOut_Num_t::DOUT6
#define DOUT7 DOut_Num_t::DOUT7
#define DOUT8 DOut_Num_t::DOUT8
#define DOUT9 DOut_Num_t::DOUT9
#define DOUT10 DOut_Num_t::DOUT10

/* Digital inputs num */
#define DIN1 DIn_Num_t::DIN1
#define DIN2 DIn_Num_t::DIN2
#define DIN3 DIn_Num_t::DIN3
#define DIN4 DIn_Num_t::DIN4
#define DIN5 DIn_Num_t::DIN5
#define DIN6 DIn_Num_t::DIN6
#define DIN7 DIn_Num_t::DIN7
#define DIN8 DIn_Num_t::DIN8
#define DIN9 DIn_Num_t::DIN9
#define DIN10 DIn_Num_t::DIN10