/**
 * @file StepperCmdHandler.cpp
 * @brief Stepper motor control functions
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "StepperCmdHandler.h"
#include "Commands.h"

static MotorNum_t _motorNums[MOTOR_MAX] = {MOTOR_1, MOTOR_2};

int StepperCmdHandler::init()
{

    int err = 0;

    SlaveController::addCmdHandler(REQUEST_MOTOR_ATTACH_LIMIT_SWITCH,
                                   [](std::vector<uint8_t> &data) {
                                       MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
                                       DIn_Num_t din    = static_cast<DIn_Num_t>(data[2]);
                                       Logic_t logic    = static_cast<Logic_t>(data[3]);
                                       MotorStepper::attachLimitSwitch(motor, din, logic);
                                       data.clear();
                                   });

    SlaveController::addCmdHandler(REQUEST_MOTOR_DETACH_LIMIT_SWITCH,
                                   [](std::vector<uint8_t> &data) {
                                       MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
                                       DIn_Num_t din    = static_cast<DIn_Num_t>(data[2]);
                                       MotorStepper::detachLimitSwitch(motor, din);
                                       data.clear();
                                   });

    SlaveController::addCmdHandler(
        REQUEST_MOTOR_SET_STEP_RESOLUTION, [](std::vector<uint8_t> &data) {
            MotorNum_t motor          = static_cast<MotorNum_t>(data[1]);
            MotorStepResolution_t res = static_cast<MotorStepResolution_t>(data[2]);
            MotorStepper::setStepResolution(motor, res);
            data.clear();
        });

    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_ACCELERATION, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float *acc       = reinterpret_cast<float *>(&data[2]);
        MotorStepper::setAcceleration(motor, *acc);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_DECELERATION, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float *dec       = reinterpret_cast<float *>(&data[2]);
        MotorStepper::setDeceleration(motor, *dec);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_MAX_SPEED, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float *speed     = reinterpret_cast<float *>(&data[2]);
        MotorStepper::setMaxSpeed(motor, *speed);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_MIN_SPEED, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float *speed     = reinterpret_cast<float *>(&data[2]);
        MotorStepper::setMinSpeed(motor, *speed);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_FULL_STEP_SPEED,
                                   [](std::vector<uint8_t> &data) {
                                       MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
                                       float *speed     = reinterpret_cast<float *>(&data[2]);
                                       MotorStepper::setFullStepSpeed(motor, *speed);
                                       data.clear();
                                   });

    SlaveController::addCmdHandler(REQUEST_MOTOR_GET_POSITION, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        int32_t position = MotorStepper::getPosition(motor);
        uint8_t *ptr     = reinterpret_cast<uint8_t *>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(int32_t));
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_GET_SPEED, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float speed      = MotorStepper::getSpeed(motor);
        uint8_t *ptr     = reinterpret_cast<uint8_t *>(&speed);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_RESET_HOME_POSITION,
                                   [](std::vector<uint8_t> &data) {
                                       MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
                                       MotorStepper::resetHomePosition(motor);
                                       data.clear();
                                   });
    
    SlaveController::addCmdHandler(REQUEST_MOTOR_SET_POSITION, [](std::vector<uint8_t> &data) {
        MotorNum_t motor   = static_cast<MotorNum_t>(data[1]);
        int32_t *position  = reinterpret_cast<int32_t *>(&data[2]);
        MotorStepper::setPosition(motor, *position);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_STOP, [](std::vector<uint8_t> &data) {
        MotorNum_t motor         = static_cast<MotorNum_t>(data[1]);
        MotorStopMode_t stopMode = static_cast<MotorStopMode_t>(data[2]);
        MotorStepper::stop(motor, stopMode);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_MOVE_ABSOLUTE, [](std::vector<uint8_t> &data) {
        MotorNum_t motor   = static_cast<MotorNum_t>(data[1]);
        uint32_t *position = reinterpret_cast<uint32_t *>(&data[2]);
        bool microStep     = static_cast<bool>(data[6]);
        MotorStepper::moveAbsolute(motor, *position, microStep);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_MOVE_RELATIVE, [](std::vector<uint8_t> &data) {
        MotorNum_t motor   = static_cast<MotorNum_t>(data[1]);
        uint32_t *position = reinterpret_cast<uint32_t *>(&data[2]);
        bool microStep     = static_cast<bool>(data[6]);
        MotorStepper::moveRelative(motor, *position, microStep);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_RUN, [](std::vector<uint8_t> &data) {
        MotorNum_t motor           = static_cast<MotorNum_t>(data[1]);
        MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
        float *speed               = reinterpret_cast<float *>(&data[3]);
        MotorStepper::run(motor, direction, *speed);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_WAIT, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        char task_name[14];
        snprintf(task_name, 14, "Wait task %i", motor);
        xTaskCreate(_waitTask, task_name, 4096, &_motorNums[motor], 9, NULL);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_MOTOR_HOMING, [](std::vector<uint8_t> &data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float *speed     = reinterpret_cast<float *>(&data[2]);
        MotorStepper::homing(motor, *speed);
        data.clear();
    });

    return err;
}

void StepperCmdHandler::_waitTask(void *pvParameters)
{
    MotorNum_t motor = *(MotorNum_t *)pvParameters;

    // Wait motor to be ready
    MotorStepper::wait(motor);

    // Send a CAN event to master
    SlaveController::sendEvent({EVENT_MOTOR_READY, (uint8_t)motor});

    // Delete task
    vTaskDelete(NULL);
}

#endif