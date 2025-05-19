/**
 * @file Stepper.h
 * @brief Stepper class definition
 * @author  Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "ModuleControl.h"
#include "DigitalDefs.h"
#include "DigitalInputs.h"
#include "DigitalInputsCmd.h"
#include "DigitalInputsCmdHandler.h"
#include "Encoder.h"
#include "EncoderCmd.h"
#include "EncoderCmdHandler.h"
#include "Module.h"
#include "MotorStepper.h"
#include "MotorStepperCmd.h"
#include "MotorStepperCmdHandler.h"
#include "MotorStepperParam.h"
#include "Slave.h"
#include "StepperConfig.h"
#include "StepperParam.h"
#include "StepperPinout.h"

#define STEPPER_ENCODER_MAX 2

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

class Stepper : public Module, public DigitalInputs, public MotorStepper, public MotorStepperParam
{
public:
    static int init(void);

    static Encoder *encoder[STEPPER_ENCODER_MAX];
};

#elif defined(MODULE_MASTER)

class Stepper : public ModuleControl, public DigitalInputsCmd, public MotorStepperCmd
{
public:
    Stepper(uint32_t sn = 0)
        : ModuleControl(TYPE_OI_STEPPER, sn), DigitalInputsCmd(this), MotorStepperCmd(this)
    {
        for (int i = 0; i < STEPPER_ENCODER_MAX; i++) {
            encoder[i] = new EncoderCmd(this, i);
        }
    }

    EncoderCmd *encoder[STEPPER_ENCODER_MAX];
};
#endif
