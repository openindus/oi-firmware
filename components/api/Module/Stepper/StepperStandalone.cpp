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

#include "StepperStandalone.h"
#include "StepperPinout.h"
#include "StepperConfig.h"
#include "StepperParam.h"

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

static const char STEPPER_TAG[] = "Stepper";

gpio_num_t _dinGpio[] = {
    STEPPER_GPIO_PIN_DIN_1,
    STEPPER_GPIO_PIN_DIN_2,
    STEPPER_GPIO_PIN_DIN_3,
    STEPPER_GPIO_PIN_DIN_4,
#if defined(CONFIG_STEPPER_VE)
    STEPPER_GPIO_PIN_DIN_5,
    STEPPER_GPIO_PIN_DIN_6
#endif
};

DigitalInput* StepperStandalone::din = new DigitalInput(_dinGpio, STEPPER_DIN_NUM);

void StepperStandalone::init()
{
    ModuleStandalone::init();

    ESP_LOGI(STEPPER_TAG, "Init");

    /* Init DIN */
    din->init();

    /* Init Motor stepper */
    ESP_LOGI(STEPPER_TAG, "Init Motor stepper");
    PS01_Hal_Config_t ps01Conf = STEPPER_CONFIG_MOTOR_DEFAULT();
    PS01_Param_t ps01Param = STEPPER_PARAM_MOTOR_DEFAULT();
    MotorStepper::init(&ps01Conf, &ps01Param, _dinGpio);
}

int StepperStandalone::digitalRead(DigitalInputNum_t dinNum)
{
    return din->digitalRead(dinNum);
}

void StepperStandalone::attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode, void* arg)
{
    din->attachInterrupt(dinNum, callback, mode, arg);
}

void StepperStandalone::detachInterrupt(DigitalInputNum_t dinNum)
{
    din->detachInterrupt(dinNum);
}

void StepperStandalone::setLimitSwitch(MotorNum_t motor, DigitalInputNum_t dinNum, DigitalInputLogic_t logic)
{
    MotorStepper::setLimitSwitch(motor, dinNum, logic);
}

void StepperStandalone::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    MotorStepper::setStepResolution(motor, res);
}

void StepperStandalone::setSpeed(MotorNum_t motor, float speed)
{
    float acc = speed * 2.0;
    float dec = speed * 2.0;
    MotorStepper::setAcceleration(motor, acc);
    MotorStepper::setDeceleration(motor, dec);
    MotorStepper::setMaxSpeed(motor, speed);
    MotorStepper::setMinSpeed(motor, 0);
    MotorStepper::setFullStepSpeed(motor, speed);
}

int32_t StepperStandalone::getPosition(MotorNum_t motor)
{
    return MotorStepper::getPosition(motor);
}

float StepperStandalone::getSpeed(MotorNum_t motor)
{
    return MotorStepper::getSpeed(motor);
}

void StepperStandalone::resetHomePosition(MotorNum_t motor)
{
    MotorStepper::resetHomePosition(motor);
}

void StepperStandalone::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    MotorStepper::stop(motor, mode);
}

void StepperStandalone::moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep)
{
    MotorStepper::moveAbsolute(motor, position, microStep);
}

void StepperStandalone::moveRelative(MotorNum_t motor, int32_t position, bool microStep)
{
    MotorStepper::moveRelative(motor, position, microStep);
}

void StepperStandalone::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    MotorStepper::run(motor, direction, speed);
}

bool StepperStandalone::isRunning(MotorNum_t motor)
{
    return MotorStepper::isRunning(motor);
}

void StepperStandalone::homing(MotorNum_t motor, float speed)
{
    MotorStepper::homing(motor, speed);
}

#endif
