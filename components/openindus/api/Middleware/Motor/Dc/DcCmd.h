/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperCmd.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Slave.h"
#include "Master.h"
#include "MotorStepper.h"

#if defined(CONFIG_MODULE_MASTER)

class DcCmd
{

public:

    DcCmd(ModuleControl* module);
    
    /**
     * @brief Send a command to run the motor in the given direction
     * 
     * @param motor 
     * @param direction
     * @param dutyCycle
     */
    void run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle);

    /**
     * @brief Send a command to stop the motor
     * 
     * @param motor 
     * @param direction
     */
    void stop(MotorNum_t motor);

private:

    ModuleControl* _module;

};

#endif