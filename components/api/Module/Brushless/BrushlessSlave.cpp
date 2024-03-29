/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessStandalone.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BrushlessSlave.h"
#include "BrushlessConfig.h"

#if defined(CONFIG_BRUSHLESS)

void BrushlessSlave::init(void)
{
    BrushlessStandalone::init();
    ModuleSlave::init();

    onRequest(CMD_MOTOR_SET_SPEED, [](RequestMsg_t msg) -> uint32_t {
        uint32_t duty_cycle = (uint32_t)msg.data;
        BrushlessStandalone::setSpeed(duty_cycle);
        return 0;
    });

    onRequest(CMD_MOTOR_SET_BRAKE, [](RequestMsg_t msg) -> uint32_t {
        bool brake = (bool)msg.data;
        BrushlessStandalone::setBrake(brake);
        return 0;
    });

    onRequest(CMD_MOTOR_SET_DIRECTION, [](RequestMsg_t msg) -> uint32_t {
        bool direction = (bool)msg.data;
        BrushlessStandalone::setDirection(direction);
        return 0;
    });

    onRequest(CMD_MOTOR_GET_SPEED, [](RequestMsg_t msg) -> uint32_t {
        float speed = BrushlessStandalone::getSpeed(); 
        uint32_t data = 0;
        memcpy(&data, &speed, sizeof(uint32_t));
        return data;  
    });

    onRequest(CMD_ENCODER_GET_SPEED, [](RequestMsg_t msg) -> uint32_t {
        float speed = BrushlessStandalone::getSpeedEncoder(); 
        uint32_t data = 0;
        memcpy(&data, &speed, sizeof(uint32_t));
        return data;   
    });

    onRequest(CMD_ENCODER_GET_DIRECTION, [](RequestMsg_t msg) -> uint32_t {
        return BrushlessStandalone::getDirection();  
    });

    onRequest(CMD_ENCODER_GET_POSITION, [](RequestMsg_t msg) -> uint32_t {
        float position = BrushlessStandalone::getPosition(); 
        uint32_t data = 0;
        memcpy(&data, &position, sizeof(uint32_t));
        return data;  
    });
}

#endif