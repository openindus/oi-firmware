/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

#include "Global.h"
#include "Motor.h"
#include "MotorStepper.h"
#include "DigitalInputs.h"

class CLI_Stepper
{
public:

    static int init(void);

private:

    static void _registerLimitSwitch(void);
    static void _registerStepResolution(void);
    static void _registerSetMaxSpeed(void);
    static void _registerGetPosition(void);
    static void _registerGetSpeed(void);
    static void _registerStop(void);
    static void _registerMoveAbsolute(void);
    static void _registerMoveRelative(void);
    static void _registerRun(void);
    static void _registerHoming(void);
};

#endif