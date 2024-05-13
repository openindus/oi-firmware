/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorStepperControl.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "MotorStepperControl.h"

MotorStepperControl::MotorStepperControl(ModuleControl* control) : _control(control) 
{
    /* Create the queue for wait function and add callback for CAN event */
    _motorWaitEvent[MOTOR_1] = xQueueCreate(1, 0);
    _motorWaitEvent[MOTOR_2] = xQueueCreate(1, 0);
}

void MotorStepperControl::attachLimitSwitch(MotorNum_t motor, DigitalInputNum_t din, DigitalInputLogic_t logic)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_ATTACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din, (uint8_t)logic};
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::detachLimitSwitch(MotorNum_t motor, DigitalInputNum_t din)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_DETACH_LIMIT_SWITCH, (uint8_t)motor, (uint8_t)din};
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setStepResolution(MotorNum_t motor, MotorStepResolution_t res)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_STEP_RESOLUTION, (uint8_t)motor, (uint8_t)res};
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setAcceleration(MotorNum_t motor, float acc)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&acc);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setDeceleration(MotorNum_t motor, float dec)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_ACCELERATION, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&dec);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setMaxSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_MAX_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setMinSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_MIN_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::setFullStepSpeed(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_FULL_STEP_SPEED, (uint8_t)motor};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

int32_t MotorStepperControl::getPosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_POSITION, (uint8_t)motor};
    _control->ctrlRequest(msgBytes);
    int32_t* position = reinterpret_cast<int32_t*>(&msgBytes[2]);
    return *position;
}

float MotorStepperControl::getSpeed(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_SPEED, (uint8_t)motor};
    _control->ctrlRequest(msgBytes);
    float* speed = reinterpret_cast<float*>(&msgBytes[2]);
    return *speed;
}

void MotorStepperControl::resetHomePosition(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_RESET_HOME_POSITION, (uint8_t)motor};
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::stop(MotorNum_t motor, MotorStopMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_STOP, (uint8_t)motor, (uint8_t)mode};
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::moveAbsolute(MotorNum_t motor, uint32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_MOVE_ABSOLUTE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    msgBytes.push_back((uint8_t)microStep);
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::moveRelative(MotorNum_t motor, int32_t position, bool microStep)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_MOVE_RELATIVE, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    msgBytes.push_back((uint8_t)microStep);
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::run(MotorNum_t motor, MotorDirection_t direction, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

void MotorStepperControl::wait(MotorNum_t motor)
{
    // Add callback
    _control->addEventCallback(EVENT_MOTOR_READY, _control->getId(), [this](uint8_t motor) {
        xQueueSend(_motorWaitEvent[motor], NULL, pdMS_TO_TICKS(100));
    });
    // Send a message to slave
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_WAIT, (uint8_t)motor};
    _control->ctrlRequest(msgBytes, false);
    // Wait for event
    xQueueReset(_motorWaitEvent[motor]);
    xQueueReceive(_motorWaitEvent[motor], NULL, portMAX_DELAY);
    // Remove event callback
    _control->removeEventCallback(EVENT_MOTOR_READY, _control->getId());

}

void MotorStepperControl::homing(MotorNum_t motor, float speed)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_HOMING, (uint8_t)motor};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

#endif