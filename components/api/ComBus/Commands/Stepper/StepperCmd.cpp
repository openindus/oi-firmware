/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperCmd.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "StepperCmd.h"

StepperCmd::StepperCmd(Controller* control) : _control(control) 
{
    /* Create the queue for wait function and add callback for CAN event */
    _motorWaitEvent[MOTOR_1] = xQueueCreate(1, 0);
    _motorWaitEvent[MOTOR_2] = xQueueCreate(1, 0);
}

void StepperCmd::attachLimitSwitch(MotorNum_t motor, DIn_Num_t din, Logic_t logic)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_ATTACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din, (uint8_t)logic};
    _control->request(msgBytes);
}

void StepperCmd::detachLimitSwitch(MotorNum_t motor, DIn_Num_t din)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_DETACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din};
    _control->request(msgBytes);
}

void StepperCmd::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_STEP_RESOLUTION, (uint8_t)motor, (uint8_t)res};
    _control->request(msgBytes);
}

void StepperCmd::setAcceleration(MotorNum_t motor, float acc)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&acc);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

void StepperCmd::setDeceleration(MotorNum_t motor, float dec)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&dec);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

void StepperCmd::setMaxSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_MAX_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

void StepperCmd::setMinSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_MIN_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

void StepperCmd::setFullStepSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_SET_FULL_STEP_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

int32_t StepperCmd::getPosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_GET_POSITION, (uint8_t)motor};
    _control->request(msgBytes);
    int32_t* position = reinterpret_cast<int32_t*>(&msgBytes[2]);
    return *position;
}

float StepperCmd::getSpeed(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_GET_SPEED, (uint8_t)motor};
    _control->request(msgBytes);
    float* speed = reinterpret_cast<float*>(&msgBytes[2]);
    return *speed;
}

void StepperCmd::resetHomePosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_RESET_HOME_POSITION, (uint8_t)motor};
    _control->request(msgBytes);
}

void StepperCmd::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_STOP, (uint8_t)motor, (uint8_t)mode};
    _control->request(msgBytes);
}

void StepperCmd::moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_MOVE_ABSOLUTE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    msgBytes.push_back((uint8_t)microStep);
    _control->request(msgBytes);
}

void StepperCmd::moveRelative(MotorNum_t motor, int32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_MOVE_RELATIVE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    msgBytes.push_back((uint8_t)microStep);
    _control->request(msgBytes);
}

void StepperCmd::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

void StepperCmd::wait(MotorNum_t motor)
{
    // Add callback (callback is rewrite at each call of wait function)
    ControllerMaster::addEventCallback(EVENT_MOTOR_READY, _control->getId(), [this](uint8_t* data) {
        xQueueSend(_motorWaitEvent[data[1]], NULL, pdMS_TO_TICKS(100));
    });

    // Send a message to slave
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_WAIT, (uint8_t)motor};
    _control->request(msgBytes, false);
    // Wait for event
    xQueueReset(_motorWaitEvent[motor]);
    xQueueReceive(_motorWaitEvent[motor], NULL, portMAX_DELAY);

}

void StepperCmd::homing(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {REQUEST_MOTOR_HOMING, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

#endif