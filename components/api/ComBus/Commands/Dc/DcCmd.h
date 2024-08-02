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

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "ControllerMaster.h"
#include "MotorStepper.h"

class DcCmd
{

public:

    DcCmd(Controller* control);
    
    /**
     * @brief Send a command to run the motor in the given direction
     * 
     * @param motor 
     * @param direction
     */
    void run(MotorNum_t motor, MotorDirection_t direction);

    /**
     * @brief Send a command to stop the motor
     * 
     * @param motor 
     * @param direction
     */
    void stop(MotorNum_t motor);

private:

    Controller* _control;

};

#endif