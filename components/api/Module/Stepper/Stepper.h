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

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

#if defined(CONFIG_MODULE_STANDALONE)
class Stepper : public StepperStandalone {};
#else 
class Stepper : public StepperSlave {};
#endif

#else
class Stepper : public StepperControl {};

#endif