/**
 * @file Stepper.h
 * @brief Stepper
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_STANDALONE)
#define Stepper StepperStandalone
#elif (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)
#define Stepper StepperSlave
#elif defined(MODULE_MASTER) 
#define Stepper StepperMaster
#endif

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

#include "StepperPinout.h"
#include "StepperParam.h"
#include "Module.h"
#include "DigitalInputs.h"
#include "MotorStepper.h"
#include "MotorStepperParam.h"

class StepperStandalone : 
    public ModuleStandalone, 
    public DigitalInputs, 
    public MotorStepper,
    public MotorStepperParam
{
public:

    static int init(void);
};

#include "CLI_DIn.h"
#include "CLI_Stepper.h"
#include "CLI_StepperParam.h"

class StepperCLI : 
    public CLI_DIn, 
    public CLI_Stepper,
    public CLI_StepperParam
{
public:

    static int init(void);
};


#endif

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)

#include "Slave.h"
#include "ResponseDIn.h"
#include "ResponseStepper.h"

class StepperSlave : 
    public Slave,
    public ResponseDIn, 
    public ResponseStepper
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "Command.h"
#include "CommandDIn.h"
#include "CommandStepper.h"

class StepperMaster : 
    public Command, 
    public CommandDIn, 
    public CommandStepper
{
public:

    StepperMaster(uint32_t sn = 0) : 
        Command(TYPE_OI_STEPPER, sn),
        CommandDIn(this),
        CommandStepper(this) {}
};
#endif


