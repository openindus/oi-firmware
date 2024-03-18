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
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_4);},
};

void StepperSlave::init(void)
{
    ModuleSlave::init();

    onRequest(REQUEST_DIGITAL_READ, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        return StepperStandalone::digitalRead(din);
    });

    onRequest(REQUEST_ATTACH_INTERRUPT, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        StepperStandalone::attachInterrupt(din, _isrCallback[din], mode);
        return 0;
    });

    onRequest(REQUEST_DETACH_INTERRUPT, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        StepperStandalone::detachInterrupt(din);
        return 0;
    });

    onRequest(REQUEST_MOTOR_STOP, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        MotorStopMode_t stopMode = (MotorStopMode_t)((msg.param & 0x00FF00) >> 8);
        StepperStandalone::stop(motor, stopMode); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_MOVE_ABSOLUTE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t position = msg.data;
        StepperStandalone::moveAbsolute(motor, position); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_MOVE_RELATIVE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t position = msg.data;
        StepperStandalone::moveRelative(motor, position); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_RUN, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        MotorDirection_t direction = (MotorDirection_t)((msg.param & 0x00FF00) >> 8);
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::run(motor, direction, speed); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_IS_RUNNING, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        return (uint32_t)StepperStandalone::isRunning(motor); 
    });

    onRequest(REQUEST_MOTOR_HOMING, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::homing(motor, speed); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_LIMIT_SWITCH, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        DigitalInputNum_t din = (DigitalInputNum_t)((msg.param & 0x00FF00) >> 8);
        DigitalInputLogic_t logic = (DigitalInputLogic_t)msg.data;
        StepperStandalone::setLimitSwitch(motor, din, logic);
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_STEP_RESOLUTION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        MotorStepResolution_t res = (MotorStepResolution_t)msg.data;
        StepperStandalone::setStepResolution(motor, res); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_MAX_SPEED, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::setMaxSpeed(motor, speed); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_MIN_SPEED, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::setMinSpeed(motor, speed); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_FULL_STEP_SPEED, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::setFullStepSpeed(motor, speed); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_ACCELERATION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float acc = 0;
        memcpy(&acc, &data, sizeof(float));
        StepperStandalone::setAcceleration(motor, acc); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_SET_DECELERATION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float dec = 0;
        memcpy(&dec, &data, sizeof(float));
        StepperStandalone::setDeceleration(motor, dec); 
        return 0;
    });

    onRequest(REQUEST_MOTOR_GET_POSITION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        int32_t position = StepperStandalone::getPosition(motor); 
        uint32_t data = 0;
        memcpy(&data, &position, sizeof(uint32_t));
        return data;
    });

    onRequest(REQUEST_MOTOR_GET_SPEED, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        float speed = StepperStandalone::getSpeed(motor); 
        uint32_t data = 0;
        memcpy(&data, &speed, sizeof(uint32_t));
        return data;
    });

    onRequest(REQUEST_MOTOR_RESET_HOME_POSITION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        StepperStandalone::resetHomePosition(motor); 
        return 0;
    });
}

#endif
