/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepper.c
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MotorStepper.h"

// static const char MOTOR_STEPPER_TAG[] = "MotorStepper";

static const gpio_num_t* _gpio;
static float _homingSpeed[MOTOR_MAX];
static DigitalInputNum_t _limitSwitchDigitalInput[MOTOR_MAX];
static DigitalInputLogic_t _limitSwitchDigitalInputLogic[MOTOR_MAX];
static bool _limitSwitchSetted[MOTOR_MAX];
static xQueueHandle _limitSwitchEvent;

static void IRAM_ATTR _limitSwitchIsr(void* arg);
static void _limitSwitchTask(void* arg);
static void _homingTask(void* arg);

int MotorStepper::init(PS01_Hal_Config_t* config, PS01_Param_t* param, const gpio_num_t* num)
{
    int err = 0;

    /* Gpio */
    _gpio = num;

    /* Config powerSTEP01 */
    PS01_Init(config, param); // TODO: watch for error

    /* Limit switch */
    _limitSwitchEvent = xQueueCreate(1, sizeof(uint32_t));
    xTaskCreate(_limitSwitchTask, "Limit switch task", 2048, NULL, 3, NULL);

    return err;
}

void MotorStepper::setLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic)
{
    _limitSwitchDigitalInput[motor] = din;
    _limitSwitchDigitalInputLogic[motor] = logic;
    _limitSwitchSetted[motor] = true;
}

void MotorStepper::setStepResolution(MotorNum_t motor, MotorStepResolution_t res) 
{
    PS01_Param_SetStepMode(motor, (PS01_StepMode_t)res);
}

void MotorStepper::setAcceleration(MotorNum_t motor, float acc)
{
    PS01_Param_SetAcc(motor, acc);
}

void MotorStepper::setDeceleration(MotorNum_t motor, float dec)
{
    PS01_Param_SetDec(motor, dec);
}

void MotorStepper::setMaxSpeed(MotorNum_t motor, float speed)
{
    PS01_Param_SetMaxSpeed(motor, speed);
}

void MotorStepper::setMinSpeed(MotorNum_t motor, float speed)
{
    min_speed_t min_speed;
    min_speed.min_speed = speed;
    PS01_Param_SetMinSpeed(motor, min_speed);
}

void MotorStepper::setFullStepSpeed(MotorNum_t motor, float speed)
{
    fs_spd_t fs_spd;
    fs_spd.fs_spd = speed;
    PS01_Param_SetFullStepSpeed(motor, fs_spd);
}

int32_t MotorStepper::getPosition(MotorNum_t motor) 
{
    return PS01_Param_GetAbsPos(motor);
}

float MotorStepper::getSpeed(MotorNum_t motor)
{
    return PS01_Param_GetSpeed(motor);
}

void MotorStepper::resetHomePosition(MotorNum_t motor) 
{
    uint32_t pos = PS01_Param_GetAbsPos(motor);
    PS01_Param_SetAbsPos(motor, pos);
}

void MotorStepper::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    switch (mode)
    {
    case SOFT_STOP:
        PS01_Cmd_SoftStop(motor);
        break;

    case HARD_STOP:
        PS01_Cmd_HardStop(motor);
        break;

    case SOFT_HIZ:
        PS01_Cmd_SoftHiZ(motor);
        break;

    case HARD_HIZ:
        PS01_Cmd_HardHiZ(motor);
        break;
    
    default:
        break;
    }
}

void MotorStepper::moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep)
{
    if (microStep == false) {
        position *= (uint8_t)(1 << (PS01_Cmd_GetParam(motor, POWERSTEP01_STEP_MODE) & 0x07));
    }
    if (position == 0) {
        PS01_Cmd_GoHome(motor);
    } else {
        PS01_Cmd_GoTo(motor, position);
    }
}

void MotorStepper::moveRelative(MotorNum_t motor, int32_t position, bool microStep)
{
    if (microStep == false) {
        position *= (uint8_t)(1 << (PS01_Cmd_GetParam(motor, POWERSTEP01_STEP_MODE) & 0x07));
    }
    if (position > 0) {
        PS01_Cmd_Move(motor, (motorDir_t)FORWARD, position);
    } else {
        PS01_Cmd_Move(motor, (motorDir_t)REVERSE, abs(position));
    }
}

void MotorStepper::run(MotorNum_t motor, MotorDirection_t direction, float speed) 
{
    uint32_t stepPerTick = (((speed)*67.108864f)+0.5f);
    PS01_Cmd_Run(motor, (motorDir_t)direction, stepPerTick);
}

bool MotorStepper::isRunning(MotorNum_t motor)
{
    return PS01_Param_GetBusyStatus(motor);
}

void MotorStepper::homing(MotorNum_t motor, float speed)
{
    _homingSpeed[motor] = speed;
    xTaskCreate(_homingTask, "Homing task", 2048, &motor, 7, NULL);
}
static void IRAM_ATTR _limitSwitchIsr(void* arg)
{
    uint32_t motor = (uint32_t)arg;
    xQueueSendFromISR(_limitSwitchEvent, &motor, NULL);
}

static void _limitSwitchTask(void* arg)
{
    MotorNum_t motor;
    while(1) {
        if(xQueueReceive(_limitSwitchEvent, &motor, portMAX_DELAY)) {
            gpio_intr_disable(_gpio[_limitSwitchDigitalInput[motor]]);
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 1);
            vTaskDelay(1/portTICK_PERIOD_MS);
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 0);
            gpio_intr_enable(_gpio[_limitSwitchDigitalInput[motor]]);
        }
    }
}

static void _homingTask(void* arg)
{
    MotorNum_t motor = *(MotorNum_t*)arg;
    DigitalInputNum_t din = _limitSwitchDigitalInput[motor];
    DigitalInputLogic_t logic = _limitSwitchDigitalInputLogic[motor];
    gpio_num_t gpio = _gpio[din];
    uint32_t stepPerTick = PS01_Speed_Steps_s_to_RegVal(_homingSpeed[motor]);
    gpio_int_type_t intrType = GPIO_INTR_DISABLE;

    /* Set switch level and releases the SW_EVN flag */
    PS01_Hal_SetSwitchLevel(motor, 0);

    /* Fetch and clear status */
    PS01_Cmd_GetStatus(motor);

    /* Attach interrupt to limit switch */
    gpio_isr_handler_add(gpio, _limitSwitchIsr, (void *)motor);
    intrType = (logic == ACTIVE_LOW) ? GPIO_INTR_NEGEDGE : GPIO_INTR_POSEDGE;
    gpio_set_intr_type(gpio, intrType);
    gpio_intr_enable(gpio);

    /* Check if motor is at home */
    if (gpio_get_level(gpio) != logic) {
        /* Perform go until command and wait */
        PS01_Cmd_GoUntil(motor, ACTION_RESET, (motorDir_t)REVERSE, stepPerTick);
        do {
            vTaskDelay(10/portTICK_PERIOD_MS);
        } while (PS01_Param_GetBusyStatus(motor));
    }

    /* Change interrupt mode */
    gpio_intr_disable(gpio);
    intrType = (logic == ACTIVE_LOW) ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE;
    gpio_set_intr_type(gpio, intrType);
    gpio_intr_enable(gpio);

    /* Perform release SW command anfd wait */
    PS01_Cmd_ReleaseSw(motor, ACTION_RESET, (motorDir_t)FORWARD);
    do {
        vTaskDelay(10/portTICK_PERIOD_MS);
    } while (PS01_Param_GetBusyStatus(motor));

    /* Detach interrupt to limit switch */
    gpio_isr_handler_remove(gpio);

    /* Delete task */
    vTaskDelete(NULL);
}
