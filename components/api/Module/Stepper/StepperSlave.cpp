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

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

IsrCallback_t StepperSlave::_isrCallback[] = {
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_4);},
};

void StepperSlave::init(void)
{
    StepperStandalone::init();
    ModuleSlave::init();

    onRequest(CMD_DIGITAL_READ, [](RequestMsg_t msg) -> uint32_t { 
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        return StepperStandalone::digitalRead(din);
    });

    onRequest(CMD_ATTACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        StepperStandalone::attachInterrupt(din, _isrCallback[din], mode); 
        return 0;
    });

    onRequest(CMD_DETACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        StepperStandalone::detachInterrupt(din); 
        return 0;
    });

    onRequest(CMD_MOTOR_STOP, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        MotorStopMode_t stopMode = (MotorStopMode_t)((msg.param & 0x00FF00) >> 8);
        StepperStandalone::stop(motor, stopMode); 
        return 0;
    });

    onRequest(CMD_MOTOR_MOVE_ABSOLUTE, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t position = msg.data;
        StepperStandalone::moveAbsolute(motor, position); 
        return 0;
    });

    onRequest(CMD_MOTOR_MOVE_RELATIVE, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t position = msg.data;
        StepperStandalone::moveRelative(motor, position); 
        return 0;
    });

    onRequest(CMD_MOTOR_RUN, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        MotorDirection_t direction = (MotorDirection_t)((msg.param & 0x00FF00) >> 8);
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::run(motor, direction, speed); 
        return 0;
    });

    onRequest(CMD_WAIT_WHILE_MOTOR_IS_RUNNING, [](RequestMsg_t msg) -> uint32_t {
        // MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        /** @todo */
        return 0;
    });

    onRequest(CMD_MOTOR_HOMING, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::homing(motor, speed); 
        return 0;
    });

    onRequest(CMD_MOTOR_SET_LIMIT_SWITCH, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)(msg.param & 0x0000FF);
        DigitalInputNum_t din = (DigitalInputNum_t)((msg.param & 0x00FF00) >> 8);
        DigitalInputLogic_t logic = (DigitalInputLogic_t)msg.data;
        StepperStandalone::setLimitSwitch(motor, din, logic); 
        return 0;
    });

    onRequest(CMD_MOTOR_SET_STEP_RESOLUTION, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        MotorStepResolution_t res = (MotorStepResolution_t)msg.data;
        StepperStandalone::setStepResolution(motor, res); 
        return 0;
    });

    onRequest(CMD_MOTOR_SET_SPEED, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        uint32_t data = msg.data;
        float speed = 0;
        memcpy(&speed, &data, sizeof(float));
        StepperStandalone::setSpeed(motor, speed); 
        return 0;
    });

    onRequest(CMD_MOTOR_GET_POSITION, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        int32_t position = StepperStandalone::getPosition(motor); 
        uint32_t data = 0;
        memcpy(&data, &position, sizeof(uint32_t));
        return data;
    });

    onRequest(CMD_MOTOR_GET_SPEED, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        float speed = StepperStandalone::getSpeed(motor); 
        uint32_t data = 0;
        memcpy(&data, &speed, sizeof(uint32_t));
        return data;
    });

    onRequest(CMD_MOTOR_RESET_HOME_POSITION, [](RequestMsg_t msg) -> uint32_t {
        MotorNum_t motor = (MotorNum_t)msg.param;
        StepperStandalone::resetHomePosition(motor); 
        return 0;
    });
}

#endif
