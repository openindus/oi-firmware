/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential_isrCallback
 * 
 * @file StepperSlave.cpp
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "StepperSlave.h"

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)

IsrCallback_t StepperSlave::_isrCallback[] = {
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

void StepperSlave::init(void)
{
    ModuleSlave::init();

    addCtrlCallback(CONTROL_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = StepperStandalone::digitalRead((DigitalInputNum_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    addCtrlCallback(CONTROL_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        StepperStandalone::attachInterrupt((DigitalInputNum_t)data[1], 
            _isrCallback[data[1]], (InterruptMode_t)data[2]);
    });

    addCtrlCallback(CONTROL_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        StepperStandalone::detachInterrupt((DigitalInputNum_t)data[1]);
    });

    addCtrlCallback(CONTROL_MOTOR_STOP, [](std::vector<uint8_t>& data) { 
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorStopMode_t stopMode = static_cast<MotorStopMode_t>(data[2]);
        StepperStandalone::stop(motor, stopMode); 
    });

    addCtrlCallback(CONTROL_MOTOR_MOVE_ABSOLUTE, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        uint32_t* position = reinterpret_cast<uint32_t*>(&data[2]);
        StepperStandalone::moveAbsolute(motor, *position); 
    });

    addCtrlCallback(CONTROL_MOTOR_MOVE_RELATIVE, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        uint32_t* position = reinterpret_cast<uint32_t*>(&data[2]);
        StepperStandalone::moveRelative(motor, *position); 
    });

    addCtrlCallback(CONTROL_MOTOR_RUN, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorDirection_t direction = static_cast<MotorDirection_t>(data[2]);
        uint32_t* speed = reinterpret_cast<uint32_t*>(&data[3]);
        StepperStandalone::run(motor, direction, *speed); 
    });

    addCtrlCallback(CONTROL_MOTOR_IS_RUNNING, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        bool isRunning = StepperStandalone::isRunning(motor); 
        data.push_back(static_cast<uint8_t>(isRunning));
    });

    addCtrlCallback(CONTROL_MOTOR_HOMING, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        uint32_t* speed = reinterpret_cast<uint32_t*>(&data[2]);
        StepperStandalone::homing(motor, *speed); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_LIMIT_SWITCH, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        DigitalInputNum_t din = static_cast<DigitalInputNum_t>(data[2]);
        DigitalInputLogic_t logic = static_cast<DigitalInputLogic_t>(data[3]);
        StepperStandalone::setLimitSwitch(motor, din, logic);
    });

    addCtrlCallback(CONTROL_MOTOR_SET_STEP_RESOLUTION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        MotorStepResolution_t res = static_cast<MotorStepResolution_t>(data[2]);
        StepperStandalone::setStepResolution(motor, res); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_MAX_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        StepperStandalone::setMaxSpeed(motor, *speed); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_MIN_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        StepperStandalone::setMinSpeed(motor, *speed); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_FULL_STEP_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* speed = reinterpret_cast<float*>(&data[2]);
        StepperStandalone::setFullStepSpeed(motor, *speed); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_ACCELERATION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* acc = reinterpret_cast<float*>(&data[2]);
        StepperStandalone::setAcceleration(motor, *acc); 
    });

    addCtrlCallback(CONTROL_MOTOR_SET_DECELERATION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float* dec = reinterpret_cast<float*>(&data[2]);
        StepperStandalone::setDeceleration(motor, *dec); 
    });

    addCtrlCallback(CONTROL_MOTOR_GET_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        int32_t position = StepperStandalone::getPosition(motor); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(int32_t));
    });

    addCtrlCallback(CONTROL_MOTOR_GET_SPEED, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        float speed = StepperStandalone::getSpeed(motor); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    addCtrlCallback(CONTROL_MOTOR_RESET_HOME_POSITION, [](std::vector<uint8_t>& data) {
        MotorNum_t motor = static_cast<MotorNum_t>(data[1]);
        StepperStandalone::resetHomePosition(motor); 
    });
}

#endif
