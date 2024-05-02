/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperStandalone.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

#include "ModuleStandalone.h"

class StepperStandalone : public ModuleStandalone, public DigitalInputs, public MotorStepper
{
public:

    static int init(void);
};

#endif