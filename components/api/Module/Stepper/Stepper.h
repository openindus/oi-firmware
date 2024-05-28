/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Stepper.h
 * @brief Functions for Stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)
#include "StepperPinout.h"
#include "StepperParam.h"
#include "DigitalInputs.h"
#include "MotorStepper.h"
#include "DigitalInputsCLI.h"
#include "MotorStepperCLI.h"
#include "MotorStepperParamCLI.h"
#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_STANDALONE)
#define Stepper StepperStandalone
#elif (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)
#define Stepper StepperSlave
#include "DigitalInputsSlave.h"
#include "MotorStepperSlave.h"
#elif defined(MODULE_MASTER) 
#define Stepper StepperControl
#include "DigitalInputsControl.h"
#include "MotorStepperControl.h"
#endif

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

class StepperCLI : public DigitalInputsCLI, public MotorStepperCLI, public MotorStepperParam
{
public:

    static int init(void);
};

class StepperStandalone : public ModuleStandalone, public DigitalInputs, public MotorStepper
{
public:

    static int init(void);
};
#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)

class StepperSlave : public ModuleSlave, public StepperStandalone,  public DigitalInputsSlave, public MotorStepperSlave
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class StepperControl : public ModuleControl, public DigitalInputsControl, public MotorStepperControl
{
public:

    StepperControl(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_STEPPER, sn),
        DigitalInputsControl(this),
        MotorStepperControl(this) {}
};
#endif


