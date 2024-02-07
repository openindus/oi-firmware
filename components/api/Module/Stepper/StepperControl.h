/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperControl.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "StepperParam.h"

#if !defined(CONFIG_STEPPER) && !defined(CONFIG_STEPPER_VE)

class StepperControl : public ModuleControl
{
public:

    StepperControl(void) : ModuleControl() {}

    int digitalRead(DigitalInputNum_t din);
    void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE);
    void detachInterrupt(DigitalInputNum_t din);

    void setLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic=ACTIVE_HIGH);
    void setStepResolution(MotorNum_t motor, MotorStepResolution_t res);
    void setMaxSpeed(MotorNum_t motor, float speed);
    void setMinSpeed(MotorNum_t motor, float speed);
    void setFullStepSpeed(MotorNum_t motor, float speed);
    void setAcceleration(MotorNum_t motor, float acc);
    void setDeceleration(MotorNum_t motor, float dec);
    int32_t getPosition(MotorNum_t motor);
    float getSpeed(MotorNum_t motor);
    void resetHomePosition(MotorNum_t motor);

    void stop(MotorNum_t motor, MotorStopMode_t mode=SOFT_HIZ);
    void moveAbsolute(MotorNum_t motor, uint32_t position);
    void moveRelative(MotorNum_t motor, int32_t position);
    void run(MotorNum_t motor, MotorDirection_t direction, float speed);
    bool isRunning(MotorNum_t motor);
    void homing(MotorNum_t motor, float speed);

private:

    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif