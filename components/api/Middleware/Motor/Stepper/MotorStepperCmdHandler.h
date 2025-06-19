/**
 * @file MotorStepperCmdHandler.h
 * @brief Stepper motor command handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "MotorStepper.h"
#include "MotorStepperParam.h"
#include "MotorStepperCmdArgs.h"
#include "Slave.h"

#define STEPPER_MOTOR_MAX   2
#define STEPPER_DIN_MAX     4

using namespace MotorStepperCmdArgs;

class MotorStepperCmdHandler
{
public:
    static inline int init(void)
    {
        Slave::addCallback(CALLBACK_MOTOR_ATTACH_LIMIT_SWITCH, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            DIn_Num_t din    = static_cast<DIn_Num_t>(data[2]);
            Logic_t logic    = static_cast<Logic_t>(data[3]);
            MotorStepper::attachLimitSwitch(motor, din, logic);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_DETACH_LIMIT_SWITCH, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            DIn_Num_t din    = static_cast<DIn_Num_t>(data[2]);
            MotorStepper::detachLimitSwitch(motor, din);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_STEP_RESOLUTION, [](std::vector<uint8_t> &data) {
                MotorNum_t motor          = static_cast<MotorNum_t>(data[1]);
                MotorStepResolution_t res = static_cast<MotorStepResolution_t>(data[2]);
                MotorStepper::setStepResolution(motor, res);
                data.clear();
            });

        Slave::addCallback(CALLBACK_MOTOR_SET_ACCELERATION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *acc       = reinterpret_cast<float *>(&data[2]);
            MotorStepper::setAcceleration(motor, *acc);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_DECELERATION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *dec       = reinterpret_cast<float *>(&data[2]);
            MotorStepper::setDeceleration(motor, *dec);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_MAX_SPEED, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *speed     = reinterpret_cast<float *>(&data[2]);
            MotorStepper::setMaxSpeed(motor, *speed);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_MIN_SPEED, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *speed     = reinterpret_cast<float *>(&data[2]);
            MotorStepper::setMinSpeed(motor, *speed);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_FULL_STEP_SPEED, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *speed     = reinterpret_cast<float *>(&data[2]);
            MotorStepper::setFullStepSpeed(motor, *speed);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_GET_POSITION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            int32_t position = MotorStepper::getPosition(motor);
            uint8_t *ptr     = reinterpret_cast<uint8_t *>(&position);
            data.insert(data.end(), ptr, ptr + sizeof(int32_t));
        });

        Slave::addCallback(CALLBACK_MOTOR_GET_SPEED, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float speed      = MotorStepper::getSpeed(motor);
            uint8_t *ptr     = reinterpret_cast<uint8_t *>(&speed);
            data.insert(data.end(), ptr, ptr + sizeof(float));
        });

        Slave::addCallback(CALLBACK_MOTOR_GET_STATUS, [](std::vector<uint8_t> &data) {
            MotorNum_t motor            = static_cast<MotorNum_t>(data[1]);
            MotorStepperStatus_t status = MotorStepper::getStatus(motor);
            uint8_t *ptr                = reinterpret_cast<uint8_t *>(&status);
            data.insert(data.end(), ptr, ptr + sizeof(MotorStepperStatus_t));
        });

        Slave::addCallback(CALLBACK_MOTOR_CLEAR_STATUS, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            MotorStepper::clearStatus(motor);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_RESET_HOME_POSITION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            MotorStepper::resetHomePosition(motor);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_POSITION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor  = static_cast<MotorNum_t>(data[1]);
            int32_t *position = reinterpret_cast<int32_t *>(&data[2]);
            MotorStepper::setPosition(motor, *position);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_STOP, [](std::vector<uint8_t> &data) {
            MotorNum_t motor         = static_cast<MotorNum_t>(data[1]);
            MotorStopMode_t stopMode = static_cast<MotorStopMode_t>(data[2]);
            MotorStepper::stop(motor, stopMode);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_MOVE_ABSOLUTE, [](std::vector<uint8_t> &data) {
            MotorNum_t motor   = static_cast<MotorNum_t>(data[1]);
            uint32_t *position = reinterpret_cast<uint32_t *>(&data[2]);
            bool microStep     = static_cast<bool>(data[6]);
            MotorStepper::moveAbsolute(motor, *position, microStep);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_MOVE_RELATIVE, [](std::vector<uint8_t> &data) {
            MotorNum_t motor   = static_cast<MotorNum_t>(data[1]);
            uint32_t *position = reinterpret_cast<uint32_t *>(&data[2]);
            bool microStep     = static_cast<bool>(data[6]);
            MotorStepper::moveRelative(motor, *position, microStep);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_RUN, [](std::vector<uint8_t> &data) {
            MotorNum_t motor           = static_cast<MotorNum_t>(data[1]);
            MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
            float *speed               = reinterpret_cast<float *>(&data[3]);
            MotorStepper::run(motor, direction, *speed);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_WAIT, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            char task_name[14];
            snprintf(task_name, 14, "Wait task %i", motor);
            xTaskCreate(_waitTask, task_name, 4096, (void *)&_motorNums[motor], 9, &_waitTaskHandler[motor]);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_HOMING, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            float *speed     = reinterpret_cast<float *>(&data[2]);
            MotorStepper::homing(motor, *speed);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_SET_ADVANCED_PARAM, [](std::vector<uint8_t> &data) {
            SetAdvancedParamArgs_s args = { static_cast<MotorNum_t>(data[1]), 
                                            static_cast<AdvancedParameter_t>(data[2]), 
                                            &data[3] };
            MotorStepperParam::setAdvancedParam(args.motor, args.advParam, args.value);
        });

        Slave::addCallback(CALLBACK_MOTOR_GET_ADVANCED_PARAM, [](std::vector<uint8_t> &data) {
            GetAdvancedParamArgs_s args = { static_cast<MotorNum_t>(data[1]), 
                                            static_cast<AdvancedParameter_t>(data[2]), 
                                            &data[3] };
            MotorStepperParam::getAdvancedParam(args.motor, args.advParam, args.value);
        });

        Slave::addCallback(CALLBACK_MOTOR_RESET_ALL_ADVANCED_PARAM, [](std::vector<uint8_t> &data) {
            MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
            MotorStepperParam::resetAllAdvancedParamPS01(motor);
            data.clear();
        });

        Slave::addCallback(CALLBACK_MOTOR_GET_SUPPLY_VOLTAGE, [](std::vector<uint8_t> &data) {
            float voltage    = MotorStepper::getSupplyVoltage();
            uint8_t *ptr     = reinterpret_cast<uint8_t *>(&voltage);
            data.insert(data.end(), ptr, ptr + sizeof(float));
        });

        Slave::addCallback(CALLBACK_MOTOR_ATTACH_FLAG_INTERRUPT, [](std::vector<uint8_t>& data) { 
            MotorStepper::attachFlagInterrupt(_flagIsrCallback);
        });
    
        Slave::addCallback(CALLBACK_MOTOR_DETACH_FLAG_INTERRUPT, [](std::vector<uint8_t>& data) { 
            MotorStepper::detachFlagInterrupt();
        });

        Slave::addResetCallback([]() {
            for (int i = 0; i < STEPPER_MOTOR_MAX; ++i) {
                // Detach all limit switches
                for (int j = 0; j < STEPPER_DIN_MAX; ++j) {
                    MotorStepper::detachLimitSwitch(static_cast<MotorNum_t>(i), static_cast<DIn_Num_t>(j));
                }
                // Delete wait task
                if (_waitTaskHandler[i] != nullptr) {
                    vTaskDelete(_waitTaskHandler[i]);
                    _waitTaskHandler[i] = nullptr;
                }
            }
        });

        Slave::addEventCallback(EVENT_CALLBACK_MOTOR_STOP, [](std::vector<uint8_t> &args) {
            MotorNum_t num       = static_cast<MotorNum_t>(args[0]);
            MotorStopMode_t mode = static_cast<MotorStopMode_t>(args[1]);
            MotorStepper::stop(num, mode);
        });

        Slave::addEventCallback(EVENT_CALLBACK_TRIGGER_LIMIT_SWITCH, [](std::vector<uint8_t> &args) {
            MotorNum_t num = static_cast<MotorNum_t>(args[0]);
            MotorStepper::triggerLimitSwitch(num);
        });

        return 0;
    }

private:
    static MotorNum_t _motorNums[MOTOR_MAX];
    static void _waitTask(void *pvParameters);
    static TaskHandle_t _waitTaskHandler[MOTOR_MAX];
    static void (*_flagIsrCallback)(MotorNum_t, MotoStepperFlag_t);
};

#endif