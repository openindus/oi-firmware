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
#include "Module.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)
#include "StepperPinout.h"
#include "StepperParam.h"
#include "DigitalInputs.h"
#include "MotorStepper.h"
#include "CLI_DIn.h"
#include "CLI_Stepper.h"
#include "CLI_StepperParam.h"
#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_STANDALONE)
#define Stepper StepperStandalone
#elif (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)
#define Stepper StepperSlave
#include "ControlSlave_DIn.h"
#include "ControlSlave_Stepper.h"
#elif defined(MODULE_MASTER) 
#define Stepper StepperControl
#include "ControlCmd_DIn.h"
#include "ControlCmd_Stepper.h"
#endif

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

class StepperCLI : public CLI_DIn, public CLI_Stepper, public MotorStepperParam
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

class StepperSlave : public ControlSlave, public StepperStandalone,  public ControlSlave_DIn, public ControlSlave_Stepper
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class StepperControl : public ControlCmd, public ControlCmd_DIn, public ControlCmd_Stepper
{
public:

    StepperControl(uint32_t sn = 0) : 
        ControlCmd(TYPE_OI_STEPPER, sn),
        ControlCmd_DIn(this),
        ControlCmd_Stepper(this) {}
};
#endif


