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
#include "StepperPinout.h"
#include "StepperParam.h"
#include "Module.h"
#include "DigitalInputs.h"
#include "MotorStepper.h"
#include "MotorStepperParam.h"
#include "ControllerSlave.h"
#include "DigitalInputsRsp.h"
#include "StepperRsp.h"
#include "Controller.h"
#include "DigitalInputsCmd.h"
#include "StepperCmd.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

class Stepper : 
    public Module, 
    public DigitalInputs, 
    public MotorStepper,
    public MotorStepperParam
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class Stepper : 
    public Controller, 
    public DigitalInputsCmd, 
    public StepperCmd
{
public:

    Stepper(uint32_t sn = 0) : 
        Controller(TYPE_OI_STEPPER, sn),
        DigitalInputsCmd(this),
        StepperCmd(this) {}
};
#endif


