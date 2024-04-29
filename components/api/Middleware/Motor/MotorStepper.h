/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepper.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Motor.h"
#include "DigitalInputs.h"
#include "powerSTEP01/PS01.h"

typedef enum {
    FULL_STEP   = 0, 
    HALF_STEP   = 1,
    STEP_1_4    = 2,
    STEP_1_8    = 3,
    STEP_1_16   = 4,
    STEP_1_32   = 5,
    STEP_1_64   = 6,
    STEP_1_128  = 7
} MotorStepResolution_t;

typedef enum {
    FORWARD = 1,
    REVERSE = 0,
} MotorDirection_t;

typedef enum {
    SOFT_STOP = 0,
    HARD_STOP,
    SOFT_HIZ,
    HARD_HIZ
} MotorStopMode_t;

class MotorStepper : public Motor
{
public:

    static void init(PS01_Hal_Config_t* config, PS01_Param_t* param, const gpio_num_t* num);
    static void setLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic);
    static void setStepResolution(MotorNum_t motor, MotorStepResolution_t res);
    static void setAcceleration(MotorNum_t motor, float acc);
    static void setDeceleration(MotorNum_t motor, float dec);
    static void setMaxSpeed(MotorNum_t motor, float speed);
    static void setMinSpeed(MotorNum_t motor, float speed);
    static void setFullStepSpeed(MotorNum_t motor, float speed);
    static int32_t getPosition(MotorNum_t motor);
    static float getSpeed(MotorNum_t motor);
    static void resetHomePosition(MotorNum_t motor);
    static void stop(MotorNum_t motor, MotorStopMode_t mode=SOFT_HIZ);
    static void moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep=true);
    static void moveRelative(MotorNum_t motor, int32_t position, bool microStep=true);
    static void run(MotorNum_t motor, MotorDirection_t direction, float speed);
    static bool isRunning(MotorNum_t motor);
    static void homing(MotorNum_t motor, float speed);
};
