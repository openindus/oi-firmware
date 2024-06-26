/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Stepper.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "StepperStandalone.h"
#include "StepperSlave.h"
#include "StepperControl.h"
#include "StepperCLI.h"
#include "StepperParamCLI.h"

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_STANDALONE)
#define Stepper StepperStandalone
#elif (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && !defined(MODULE_STANDALONE)
#define Stepper StepperSlave
#else 
#define Stepper StepperControl
#endif