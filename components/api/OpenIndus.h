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

/* Core */
#if defined(OI_CORE) && defined(MODULE_STANDALONE)
#define OICore CoreStandalone
#else 
#define OICore CoreMaster
#endif

/* Discrete */
#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_STANDALONE)
#define OIDiscrete DiscreteStandalone
#elif (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && !defined(MODULE_STANDALONE)
#define OIDiscrete DiscreteSlave
#else 
#define OIDiscrete DiscreteControl
#endif

/* Mixed */
#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define OIMixed MixedStandalone
#elif defined(OI_MIXED) && !defined(MODULE_STANDALONE)
#define OIMixed MixedSlave
#else 
#define OIMixed MixedControl
#endif

/* Stepper */
#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_STANDALONE)
#define OIStepper StepperStandalone
#elif (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && !defined(MODULE_STANDALONE)
#define OIStepper StepperSlave
#else 
#define OIStepper StepperControl
#endif

/* Relay */
#if (defined(OI_RELAY_HP) || defined(OI_RELAY_LP)) && defined(MODULE_STANDALONE)
#define OIRelay RelayStandalone
#elif (defined(OI_RELAY_HP) || defined(OI_RELAY_LP)) && !defined(MODULE_STANDALONE)
#define OIRelay RelaySlave
#else 
#define OIRelay RelayControl
#endif

/* Brushless */
#if defined(OI_BRUSHLESS) && defined(MODULE_STANDALONE)
#define OIBrushless BrushlessStandalone
#elif defined(OI_BRUSHLESS) && !defined(MODULE_STANDALONE)
#define OIBrushless BrushlessSlave
#else 
#define OIBrushless BrushlessControl
#endif

/* Analogls */
#if defined(OI_ANALOG_LS) && defined(MODULE_STANDALONE)
#define OIAnalogls AnaloglsStandalone
#elif defined(OI_ANALOG_LS) && !defined(MODULE_STANDALONE)
#define OIAnalogls AnaloglsSlave
#else 
#define OIAnalogls AnaloglsControl
#endif
