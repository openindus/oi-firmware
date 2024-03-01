/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleStepper.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "StepperStandalone.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

class ConsoleStepper
{
public:

    static void registerCommand(void);

private:

    static void _registerDigitalRead(void);
    static void _registerAttachInterrupt(void);
    static void _registerDetachInterrupt(void);
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
