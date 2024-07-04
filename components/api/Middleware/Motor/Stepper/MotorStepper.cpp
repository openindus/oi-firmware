/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepper.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MotorStepper.h"

static const char TAG[] = "MotorStepper";

static float _homingSpeed[MOTOR_MAX];


static std::vector<std::pair<DIn_Num_t, Logic_t>> _limitSwitchDigitalInput[MOTOR_MAX]; 

static MotorNum_t _motorNums[MOTOR_MAX] = {MOTOR_1, MOTOR_2};

static xQueueHandle _busyEvent[MOTOR_MAX];
static xSemaphoreHandle _homingSemaphore[MOTOR_MAX];

static bool _taskHomingStopRequested[MOTOR_MAX] = {false, false};
static xSemaphoreHandle _taskHomingStopSemaphore[MOTOR_MAX];

static void _digitalInterruptHandler(void* arg);
static void _homingTask(void* arg);
static void _busyCallback(uint8_t motor);

int MotorStepper::init(PS01_Hal_Config_t* config, PS01_Param_t* param)
{
    int err = 0;

    /* Config powerSTEP01 */
    PS01_Init(config, param); // TODO: watch for error

    /* Attach busy pin callbacks */
    _busyEvent[MOTOR_1] = xQueueCreate(1, 0);
    _busyEvent[MOTOR_2] = xQueueCreate(1, 0);
    PS01_Hal_AttachBusyInterrupt(&_busyCallback);

    /* Create semaphore for homing task */
    _homingSemaphore[MOTOR_1] = xSemaphoreCreateMutex();
    xSemaphoreGive(_homingSemaphore[MOTOR_1]);
    _homingSemaphore[MOTOR_2] = xSemaphoreCreateMutex();
    xSemaphoreGive(_homingSemaphore[MOTOR_2]);
    _taskHomingStopSemaphore[MOTOR_1] = xSemaphoreCreateMutex();
    xSemaphoreGive(_taskHomingStopSemaphore[MOTOR_1]);
    _taskHomingStopSemaphore[MOTOR_2] = xSemaphoreCreateMutex();
    xSemaphoreGive(_taskHomingStopSemaphore[MOTOR_2]);

    return err;
}

void MotorStepper::attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic)
{   
    /* Remove sensor if it was already added to the list */
    auto it = std::find_if( _limitSwitchDigitalInput[motor].begin(), _limitSwitchDigitalInput[motor].end(),
        [&din](const std::pair<DIn_Num_t, Logic_t>& element){ return element.first == din;} );

    if(it != _limitSwitchDigitalInput[motor].end()) _limitSwitchDigitalInput[motor].erase(it);

    /* Store limit switch info */
    _limitSwitchDigitalInput[motor].push_back({din, logic});

    /* Attach interrupt to limit switch */
    DigitalInputs::attachInterrupt(din, _digitalInterruptHandler, (logic == ACTIVE_HIGH) ? RISING_MODE : FALLING_MODE, &_motorNums[motor]);
}

void MotorStepper::detachLimitSwitch(MotorNum_t motor, DIn_Num_t din) 
{
    /* Find the element in the vector */
    auto it = std::find_if( _limitSwitchDigitalInput[motor].begin(), _limitSwitchDigitalInput[motor].end(),
        [&din](const std::pair<DIn_Num_t, Logic_t>& element){ return element.first == din;} );

    /* Remove the element if found */
    if(it != _limitSwitchDigitalInput[motor].end()) _limitSwitchDigitalInput[motor].erase(it);

    /* Remove the interrupt */
    DigitalInputs::detachInterrupt(din);
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

    // Stop homing task if it was in progress
    if (xSemaphoreTake(_homingSemaphore[motor], 0) == pdFALSE) {
        xSemaphoreTake(_taskHomingStopSemaphore[motor], portMAX_DELAY);
        _taskHomingStopRequested[motor] = true;
        xSemaphoreGive(_taskHomingStopSemaphore[motor]);
    } else {
        xSemaphoreGive(_homingSemaphore[motor]);
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

void MotorStepper::wait(MotorNum_t motor)
{
    // Wait for motor to start
    vTaskDelay(pdTICKS_TO_MS(1));

    // Wait for homing to finish
    xSemaphoreTake(_homingSemaphore[motor], portMAX_DELAY);
    xSemaphoreGive(_homingSemaphore[motor]);

    // Empty the queue is a precedent interrupt happened
    xQueueReset(_busyEvent[motor]);

    // Check if status if not already high
    while (!PS01_Hal_GetBusyLevel(motor)) {
        // Wait for an interrupt (rising edge) on busy pin
        xQueueReceive(_busyEvent[motor], NULL, pdMS_TO_TICKS(500));
    }
}

void MotorStepper::homing(MotorNum_t motor, float speed)
{
    _homingSpeed[motor] = speed;
    char task_name[16];
    snprintf(task_name, 16, "Homing task %i", motor);   
    xTaskCreate(_homingTask, task_name, 4096, &_motorNums[motor], 7, NULL);
}

static void _digitalInterruptHandler(void *arg)
{
    MotorNum_t motor = *(MotorNum_t*)arg;
    PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 1);
    vTaskDelay(pdTICKS_TO_MS(1));
    PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 0);
}

static void _homingTask(void* arg)
{
    MotorNum_t motor = *(MotorNum_t*)arg;

    ESP_LOGI(TAG, "Launch homing for MOTOR_%i", motor+1);

    xSemaphoreTake(_homingSemaphore[motor], portMAX_DELAY);

    if (_limitSwitchDigitalInput[motor].size() == 0) {
        ESP_LOGE(TAG, "Please attach a limit switch before homing !");
        vTaskDelete(NULL);
    }

    DIn_Num_t din = _limitSwitchDigitalInput[motor].front().first;
    Logic_t logic = _limitSwitchDigitalInput[motor].front().second;
    uint32_t stepPerTick = PS01_Speed_Steps_s_to_RegVal(_homingSpeed[motor]);

    xSemaphoreTake(_taskHomingStopSemaphore[motor], portMAX_DELAY);
    _taskHomingStopRequested[motor] = false;
    xSemaphoreGive(_taskHomingStopSemaphore[motor]);

    /* Check if motor is at home */
    if (DigitalInputs::digitalRead(din) != logic) {
        /* Perform go until command and wait */
        PS01_Cmd_GoUntil(motor, ACTION_RESET, (motorDir_t)REVERSE, stepPerTick);
        /* Empty the queue if a precedent interrupt happened */
        xQueueReset(_busyEvent[motor]);
        /* Check if status if not already high */
        if (!PS01_Hal_GetBusyLevel(motor)) {
            /* Wait for an interrupt (rising edge) on busy pin */
            xQueueReceive(_busyEvent[motor], NULL, portMAX_DELAY);
        }
    }

    /* Invert the switch logic to stop the motor when it leaves the sensor */
    DigitalInputs::attachInterrupt(din, _digitalInterruptHandler, (logic == ACTIVE_HIGH) ? FALLING_MODE : RISING_MODE, &_motorNums[motor]);

    /* If a stop command was issued, do not launch the second action --> homing is aborted */
    xSemaphoreTake(_taskHomingStopSemaphore[motor], portMAX_DELAY);
    bool stop = _taskHomingStopRequested[motor];
    xSemaphoreGive(_taskHomingStopSemaphore[motor]);
    
    if (!stop) {
        /* Perform release SW command and wait */
        PS01_Cmd_ReleaseSw(motor, ACTION_RESET, (motorDir_t)FORWARD);
        /* Empty the queue is a precedent interrupt happened */
        xQueueReset(_busyEvent[motor]);
        /* Check if status if not already high */
        if (!PS01_Hal_GetBusyLevel(motor)) {
            /* Wait for an interrupt (rising edge) on busy pin */
            xQueueReceive(_busyEvent[motor], NULL, portMAX_DELAY);
        }
    } else {
        ESP_LOGW(TAG, "Homing aborted for MOTOR_%i", motor+1);    
    }

    /* Re-invert the switch logic */
    DigitalInputs::attachInterrupt(din, _digitalInterruptHandler, (logic == ACTIVE_HIGH) ? RISING_MODE : FALLING_MODE, &_motorNums[motor]);

    /* Release semaphore */
    xSemaphoreGive(_homingSemaphore[motor]);

    /* Delete task */
    vTaskDelete(NULL);
}

static void _busyCallback(uint8_t motor)
{
    xQueueSend(_busyEvent[motor], NULL, pdMS_TO_TICKS(10));
}