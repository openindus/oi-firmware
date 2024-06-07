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
#define Stepper StepperCmd
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

#include "BusCtrlSlave.h"
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_Stepper.h"

class StepperSlave : 
    public BusCtrlSlave,
    public BusCtrlSlave_DIn, 
    public BusCtrlSlave_Stepper
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "BusCtrlCmd.h"
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_Stepper.h"

class StepperCmd : 
    public BusCtrlCmd, 
    public BusCtrlCmd_DIn, 
    public BusCtrlCmd_Stepper
{
public:

    StepperCmd(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_STEPPER, sn),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_Stepper(this) {}
};
#endif


