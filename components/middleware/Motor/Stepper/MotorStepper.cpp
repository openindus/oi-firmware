/**
 * @file MotorStepper.cpp
 * @brief MotorStepper class implementation
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorStepper.h"
#include <vector>
#include <algorithm>
#include "esp_log.h"
#include "driver/adc.h"

static const char TAG[] = "MotorStepper";

static float _homingSpeed[MOTOR_MAX];
static std::vector<std::pair<DIn_Num_t, Logic_t>> _limitSwitchDigitalInput[MOTOR_MAX]; 
static MotorNum_t _motorNums[MOTOR_MAX] = {MOTOR_1, MOTOR_2};
static QueueHandle_t _busyEvent[MOTOR_MAX];
static SemaphoreHandle_t _homingSemaphore[MOTOR_MAX];
static bool _taskHomingStopRequested[MOTOR_MAX] = {false, false};
static SemaphoreHandle_t _taskHomingStopSemaphore[MOTOR_MAX];

static void _triggerLimitSwitch(void* arg);
static void _homingTask(void* arg);
static void _busyCallback(uint8_t motor);
static void _configProtections(void);
static void(*_flagIsrCallback)(MotorNum_t, MotoStepperFlag_t) = nullptr;

int MotorStepper::init(PS01_Hal_Config_t* config, PS01_Param_t* param)
{
    int err = 0;

    /* Config powerSTEP01 */
    PS01_Init(config, param); // TODO: watch for error

    /* Protections powerSTEP01 */
    _configProtections();

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

    /* CLI */
    _registerCLI();

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
    DigitalInputs::attachInterrupt(din, _triggerLimitSwitch, (logic == ACTIVE_HIGH) ? RISING_MODE : FALLING_MODE, &_motorNums[motor]);
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

void MotorStepper::setPosition(MotorNum_t motor, int32_t position)
{
    PS01_Param_SetAbsPos(motor, position);
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

float MotorStepper::getSupplyVoltage(void)
{
    float voltage = 0.0f;
    int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
    if (adc_value > 0) {
        voltage = (float)adc_value * 3.3f / 4095.0f;
    }
    return ((voltage * (510.0f + 4300.0f)) / 510.0f); // R1 = 510 Ohm, R2 = 4300 Ohm
}

void MotorStepper::attachFlagInterrupt(void(*callback)(MotorNum_t, MotoStepperFlag_t))
{
    _flagIsrCallback = callback;
    
    auto wrapper = [](uint8_t motor, uint16_t status) {
        if (!_flagIsrCallback) return;

        if (((status & POWERSTEP01_STATUS_CMD_ERROR) >> 7) == 1) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_CMD_ERROR);
        }
        if (((status & POWERSTEP01_STATUS_UVLO) >> 9) == 0) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_UVLO);
        }
        if (((status & POWERSTEP01_STATUS_UVLO_ADC) >> 10) == 0) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_UVLO_ADC);
        }
        if (((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b01) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_TH_WARNING);
        }
        if (((((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b10) ||
            ((status & POWERSTEP01_STATUS_TH_STATUS) >> 11) == 0b11)) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_TH_SHUTDOWN);
        }
        if (((status & POWERSTEP01_STATUS_OCD) >> 13) == 0) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_OCD);
        }
        if (((status & POWERSTEP01_STATUS_STALL_A) >> 14) == 0) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_STALL_A);
        }
        if (((status & POWERSTEP01_STATUS_STALL_B) >> 15) == 0) {
            _flagIsrCallback((MotorNum_t)motor, FLAG_STALL_B);
        }
    };

    PS01_Hal_AttachFlagInterrupt((void(*)(uint8_t, uint16_t))wrapper);
}
void MotorStepper::detachFlagInterrupt(void)
{
    PS01_Hal_DetachFlagInterrupt();
}

void MotorStepper::triggerLimitSwitch(MotorNum_t motor)
{
    PS01_Hal_SetSwitchLevel(motor, 1);
    vTaskDelay(pdTICKS_TO_MS(1));
    PS01_Hal_SetSwitchLevel(motor, 0);
}

static void _triggerLimitSwitch(void *arg)
{
    MotorNum_t motor = *(MotorNum_t*)arg;
    MotorStepper::triggerLimitSwitch(motor);
}

static void _homingTask(void* arg)
{
    MotorNum_t motor = *(MotorNum_t*)arg;

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
        while (!PS01_Hal_GetBusyLevel(motor) && (DigitalInputs::digitalRead(din) != logic)) {
            /* Wait for an interrupt (rising edge) on busy pin */
            xQueueReceive(_busyEvent[motor], NULL, pdMS_TO_TICKS(10));
        }
        /* If motor was not stopped by interrupt, stop it */
        if (!PS01_Hal_GetBusyLevel(motor)) {
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 1);
            vTaskDelay(pdTICKS_TO_MS(1));
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 0);
        }
    }

    /* Invert the switch logic to stop the motor when it leaves the sensor */
    DigitalInputs::attachInterrupt(din, _triggerLimitSwitch, (logic == ACTIVE_HIGH) ? FALLING_MODE : RISING_MODE, &_motorNums[motor]);

    /* If a stop command was issued, do not launch the second action --> homing is aborted */
    xSemaphoreTake(_taskHomingStopSemaphore[motor], portMAX_DELAY);
    bool stop = _taskHomingStopRequested[motor];
    xSemaphoreGive(_taskHomingStopSemaphore[motor]);
    
    if (!stop) {
        /* Perform release SW command and wait */
        PS01_Cmd_ReleaseSw(motor, ACTION_RESET, (motorDir_t)FORWARD);
        /* Check if motor is not already out of limitswitch (sometimes motor is already out of limitswitch after the releaseSw command and motor will run to infinite) */
        if (DigitalInputs::digitalRead(din) != logic) {
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 1);
            vTaskDelay(pdTICKS_TO_MS(1));
            PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 0);
        } 
        /* Else wait for a busy event */
        else {
            /* Empty the queue is a precedent interrupt happened */
            xQueueReset(_busyEvent[motor]);
            /* Check if status if not already high */
            while (!PS01_Hal_GetBusyLevel(motor) && (DigitalInputs::digitalRead(din) == logic)) {
                /* Wait for an interrupt (rising edge) on busy pin */
                xQueueReceive(_busyEvent[motor], NULL, pdMS_TO_TICKS(10));
            }
            /* If motor was not stopped by interrupt, stop it */
            if (!PS01_Hal_GetBusyLevel(motor)) {
                PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 1);
                vTaskDelay(pdTICKS_TO_MS(1));
                PS01_Hal_SetSwitchLevel((MotorNum_t)motor, 0);
            }
        }
    } else {
        ESP_LOGW(TAG, "Homing aborted for MOTOR_%i", motor+1);    
    }

    /* Re-invert the switch logic */
    DigitalInputs::attachInterrupt(din, _triggerLimitSwitch, (logic == ACTIVE_HIGH) ? RISING_MODE : FALLING_MODE, &_motorNums[motor]);

    /* Release semaphore */
    xSemaphoreGive(_homingSemaphore[motor]);

    /* Delete task */
    vTaskDelete(NULL);
}

static void _busyCallback(uint8_t motor)
{
    xQueueSend(_busyEvent[motor], NULL, pdMS_TO_TICKS(10));
}

static void _configProtections(void)
{
    /* Configure ADC to read power supply voltage */
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
    ESP_LOGD(TAG, "ADC Value on GPIO1: %d", adc_value);

    /* Configure PS01 protections for all motors */
    for (int i=0; i<MOTOR_MAX; i++) {
        /* VCCVAL - depends on supply voltage */
        register_config_t config = PS01_Param_GetConfig((MotorNum_t)i);
        if (adc_value < 1911) { // Power supply volatge < 15V
            config.vm.vccval = 0;
        } else { // Power supply volatge > 15V
            config.vm.vccval = 1;
        }
        PS01_Param_SetConfig(MOTOR_1, config);

        /* TVAL - must not exceed 0.3V */
        if (PS01_Param_GetTvalAcc((MotorNum_t)i) > 0.3) PS01_Param_SetTvalAcc((MotorNum_t)i, 0.3);
        if (PS01_Param_GetTvalDec((MotorNum_t)i) > 0.3) PS01_Param_SetTvalDec((MotorNum_t)i, 0.3);
        if (PS01_Param_GetTvalRun((MotorNum_t)i) > 0.3) PS01_Param_SetTvalRun((MotorNum_t)i, 0.3);
        if (PS01_Param_GetTvalHold((MotorNum_t)i) > 0.3) PS01_Param_SetTvalHold((MotorNum_t)i, 0.3);
    }    
}