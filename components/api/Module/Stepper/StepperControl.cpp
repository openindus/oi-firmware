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

#if !defined(OI_STEPPER) && !defined(OI_STEPPER_VE)

int StepperControl::digitalRead(DigitalInputNum_t din) 
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DIGITAL_READ;
    msg.param = (uint16_t)din;
    return (int)request(msg);
}

void StepperControl::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ATTACH_INTERRUPT;
    msg.param = (uint16_t)din;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[din] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(EVENT_DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void StepperControl::detachInterrupt(DigitalInputNum_t din)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DETACH_INTERRUPT;
    msg.param = (uint16_t)din;
    request(msg);
}

void StepperControl::setLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_LIMIT_SWITCH;
    msg.param = (uint16_t)((din << 8) | motor);
    msg.data = (uint32_t)logic;
    request(msg);
}

void StepperControl::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_STEP_RESOLUTION;
    msg.param = (uint16_t)motor;
    msg.data = (uint32_t)res;
    request(msg);
}

void StepperControl::setMaxSpeed(MotorNum_t motor, float speed)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_MAX_SPEED;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &speed, sizeof(uint32_t));
    request(msg);
}

void StepperControl::setMinSpeed(MotorNum_t motor, float speed)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_MIN_SPEED;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &speed, sizeof(uint32_t));
    request(msg);
}

void StepperControl::setFullStepSpeed(MotorNum_t motor, float speed)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_FULL_STEP_SPEED;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &speed, sizeof(uint32_t));
    request(msg);
}

void StepperControl::setAcceleration(MotorNum_t motor, float acc)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_ACCELERATION;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &acc, sizeof(uint32_t));
    request(msg);
}

void StepperControl::setDeceleration(MotorNum_t motor, float dec)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_SET_DECELERATION;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &dec, sizeof(uint32_t));
    request(msg);
}

int32_t StepperControl::getPosition(MotorNum_t motor)
{
    uint32_t data;
    int32_t position;
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_GET_POSITION;
    msg.param = (uint16_t)motor;
    data = request(msg);
    memcpy(&position, &data, sizeof(int32_t));
    return position;
}

float StepperControl::getSpeed(MotorNum_t motor)
{
    uint32_t data;
    float speed;
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_GET_SPEED;
    msg.param = (uint16_t)motor;
    data = request(msg);
    memcpy(&speed, &data, sizeof(float));
    return speed;
}

void StepperControl::resetHomePosition(MotorNum_t motor)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_RESET_HOME_POSITION;
    msg.param = (uint16_t)motor;
    request(msg);
}

void StepperControl::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_STOP;
    msg.param = (uint16_t)motor;
    msg.data = (uint32_t)mode;
    request(msg);
}

void StepperControl::moveAbsolute(MotorNum_t motor, uint32_t position)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_MOVE_ABSOLUTE;
    msg.param = (uint16_t)motor;
    msg.data = (uint32_t)position;
    request(msg);
}

void StepperControl::moveRelative(MotorNum_t motor, int32_t position)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_MOVE_RELATIVE;
    msg.param = (uint16_t)motor;
    msg.data = (uint32_t)position;
    request(msg);
}

void StepperControl::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_RUN;
    msg.param = (uint16_t)((direction << 8) | motor);
    memcpy(&msg.data, &speed, sizeof(uint32_t));
    request(msg);
}

bool StepperControl::isRunning(MotorNum_t motor)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_IS_RUNNING;
    msg.param = (uint16_t)motor;
    return (bool)request(msg);
}

void StepperControl::homing(MotorNum_t motor, float speed)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_MOTOR_HOMING;
    msg.param = (uint16_t)motor;
    memcpy(&msg.data, &speed, sizeof(uint32_t));
    request(msg);
}

#endif
