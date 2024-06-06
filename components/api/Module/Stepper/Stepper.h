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
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_Stepper.h"
#elif defined(MODULE_MASTER) 
#define Stepper StepperBusCtrl
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_Stepper.h"
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

class StepperSlave : public BusCtrlSlave, public StepperStandalone,  public BusCtrlSlave_DIn, public BusCtrlSlave_Stepper
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class StepperBusCtrl : public BusCtrlCmd, public BusCtrlCmd_DIn, public BusCtrlCmd_Stepper
{
public:

    StepperBusCtrl(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_STEPPER, sn),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_Stepper(this) {}
};
#endif


