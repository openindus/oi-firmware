/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepperSlave.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_SLAVE)

#include "MotorStepperSlave.h"

static MotorNum_t _motorNums[MOTOR_MAX] = {MOTOR_1, MOTOR_2};

int MotorStepperSlave::init() {
    
    int err = 0;

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_ATTACH_LIMIT_SWITCH, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        DigitalInputNum_t din = static_cast<DigitalInputNum_t>(data[2]);
        DigitalInputLogic_t logic = static_cast<DigitalInputLogic_t>(data[3]);
        MotorStepper::attachLimitSwitch(motor, din, logic);
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_DETACH_LIMIT_SWITCH, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        DigitalInputNum_t din = static_cast<DigitalInputNum_t>(data[2]);
        MotorStepper::detachLimitSwitch(motor, din);
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_STEP_RESOLUTION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorStepResolution_t res = static_cast<MotorStepResolution_t>(data[2]);
        MotorStepper::setStepResolution(motor, res);
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_ACCELERATION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* acc = reinterpret_cast<float*>(&data[2]);
        MotorStepper::setAcceleration(motor, *acc); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_DECELERATION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* dec = reinterpret_cast<float*>(&data[2]);
        MotorStepper::setDeceleration(motor, *dec); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_MAX_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        MotorStepper::setMaxSpeed(motor, *speed); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_MIN_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        MotorStepper::setMinSpeed(motor, *speed); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_SET_FULL_STEP_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        MotorStepper::setFullStepSpeed(motor, *speed); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_GET_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        int32_t position = MotorStepper::getPosition(motor); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(int32_t));
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_GET_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float speed = MotorStepper::getSpeed(motor); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_RESET_HOME_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorStepper::resetHomePosition(motor); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_STOP, [](std::vector<uint8_t>& data) { 
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorStopMode_t stopMode = static_cast<MotorStopMode_t>(data[2]);
        MotorStepper::stop(motor, stopMode); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_MOVE_ABSOLUTE, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        uint32_t* position = reinterpret_cast<uint32_t*>(&data[2]);
        bool microStep = static_cast<bool>(data[3]);
        MotorStepper::moveAbsolute(motor, *position, microStep); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_MOVE_RELATIVE, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        uint32_t* position = reinterpret_cast<uint32_t*>(&data[2]);
        bool microStep = static_cast<bool>(data[3]);
        MotorStepper::moveRelative(motor, *position, microStep); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_RUN, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
        float* speed = reinterpret_cast<float*>(&data[3]);
        MotorStepper::run(motor, direction, *speed); 
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_WAIT, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        char task_name[14];
        snprintf(task_name, 14, "Wait task %i", motor);   
        xTaskCreate(_waitTask, task_name, 4096, &_motorNums[motor], 9, NULL);
    });

    ModuleSlave::addCtrlCallback(CONTROL_MOTOR_HOMING, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        MotorStepper::homing(motor, *speed); 
    });

    return err;
}

void MotorStepperSlave::_waitTask(void *pvParameters)
{
    MotorNum_t motor = *(MotorNum_t*)pvParameters;

    // Wait motor to be ready
    MotorStepper::wait(motor);

    // Send a CAN event to master
    ModuleSlave::sendEvent({EVENT_MOTOR_READY, (uint8_t)motor});

    // Delete task
    vTaskDelete(NULL);
}

#endif