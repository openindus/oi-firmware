/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperStandalone.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

#if defined(CONFIG_STEPPER)
#define STEPPER_DIN_NUM    4
#define STEPPER_MOTOR_NUM   2
#else
#define STEPPER_DIN_NUM    6
#define STEPPER_MOTOR_NUM   1
#endif

class StepperStandalone : public ModuleStandalone
{
public:

    /* DigitalInput */
    static int digitalRead(DigitalInputNum_t etorNum);
    static void attachInterrupt(DigitalInputNum_t etorNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    static void detachInterrupt(DigitalInputNum_t etorNum);

    /* Motor */
    static void setLimitSwitch(MotorNum_t motor, DigitalInputNum_t etorNum, DigitalInputLogic_t logic=ACTIVE_HIGH);
    static void setStepResolution(MotorNum_t motor, MotorStepResolution_t res);
    static void setSpeed(MotorNum_t motor, float speed);
    static int32_t getPosition(MotorNum_t motor);
    static float getSpeed(MotorNum_t motor);
    static void resetHomePosition(MotorNum_t motor);
    static void stop(MotorNum_t motor, MotorStopMode_t mode=SOFT_HIZ);
    static void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep=true);
    static void moveRelative(MotorNum_t motor, int32_t position, bool microStep=true);
    static void run(MotorNum_t motor, MotorDirection_t direction, float speed);
    static void waitWhileMotorIsRunning(MotorNum_t motor);
    static void homing(MotorNum_t motor, float speed);

// protected:

    static void init(void);

private:

    static DigitalInput* etor;
    static Motor* motor;

};

#endif