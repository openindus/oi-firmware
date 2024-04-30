/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperControl.cpp
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "StepperControl.h"

#if defined(MODULE_MASTER)

int StepperControl::digitalRead(DigitalInputNum_t din) 
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_READ, (uint8_t)din};
    ctrlRequest(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void StepperControl::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ATTACH_INTERRUPT, (uint8_t)din, (uint8_t)mode};
    _isrCallback[din] = callback;
    ctrlRequest(msgBytes);
    
    addEventCallback(
        EVENT_DIGITAL_INTERRUPT, 
        _id, 
        [this](uint8_t num) { _isrCallback[num](NULL); }
    );
}

void StepperControl::detachInterrupt(DigitalInputNum_t din)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DETACH_INTERRUPT, (uint8_t)din};
    ctrlRequest(msgBytes);
}

void StepperControl::setLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din, (uint8_t)logic};
    ctrlRequest(msgBytes);
}

void StepperControl::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_STEP_RESOLUTION, (uint8_t)motor, (uint8_t)res};
    ctrlRequest(msgBytes);
}

void StepperControl::setMaxSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_MAX_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

void StepperControl::setMinSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_MIN_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

void StepperControl::setFullStepSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_FULL_STEP_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

void StepperControl::setAcceleration(MotorNum_t motor, float acc)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&acc);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

void StepperControl::setDeceleration(MotorNum_t motor, float dec)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&dec);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

int32_t StepperControl::getPosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_POSITION, (uint8_t)motor};
    ctrlRequest(msgBytes);
    int32_t* position = reinterpret_cast<int32_t*>(&msgBytes[2]);
    return *position;
}

float StepperControl::getSpeed(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_SPEED, (uint8_t)motor};
    ctrlRequest(msgBytes);
    float* speed = reinterpret_cast<float*>(&msgBytes[2]);
    return *speed;
}

void StepperControl::resetHomePosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_RESET_HOME_POSITION, (uint8_t)motor};
    ctrlRequest(msgBytes);
}

void StepperControl::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_STOP, (uint8_t)motor, (uint8_t)mode};
    ctrlRequest(msgBytes);
}

void StepperControl::moveAbsolute(MotorNum_t motor, uint32_t position)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_MOVE_ABSOLUTE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    ctrlRequest(msgBytes);
}

void StepperControl::moveRelative(MotorNum_t motor, int32_t position)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_MOVE_RELATIVE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    ctrlRequest(msgBytes);
}

void StepperControl::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

bool StepperControl::isRunning(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_IS_RUNNING, (uint8_t)motor};
    ctrlRequest(msgBytes);
    return static_cast<bool>(msgBytes[2]);
}

void StepperControl::homing(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_HOMING, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

#endif
