/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessControl.cpp
 * @brief Functions for brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "BrushlessControl.h"

#if !defined(CONFIG_BRUSHLESS)

void BrushlessControl::setSpeed(uint32_t duty_cycle)
{
    RequestMsg_t msg;
    msg.cmd = CMD_MOTOR_SET_SPEED;
    msg.data = (uint32_t)duty_cycle;
    request(msg);
}

void BrushlessControl::setBrake(bool brake)
{
    RequestMsg_t msg;
    msg.cmd = CMD_MOTOR_SET_BRAKE;
    msg.data = (bool)brake;
    request(msg);
}

void BrushlessControl::setDirection(bool direction)
{
    RequestMsg_t msg;
    msg.cmd = CMD_MOTOR_SET_DIRECTION;
    msg.data = (bool)direction;
    request(msg);
}

float BrushlessControl::getSpeed(void)
{
    uint32_t data;
    float speed;
    RequestMsg_t msg;
    msg.cmd = CMD_MOTOR_GET_SPEED;
    data = request(msg);
    memcpy(&speed, &data, sizeof(float));
    return speed;
}

float BrushlessControl::getSpeedEncoder(void)
{
    uint32_t data;
    float speed;
    RequestMsg_t msg;
    msg.cmd = CMD_ENCODER_GET_SPEED;
    data = request(msg);
    memcpy(&speed, &data, sizeof(float));
    return speed;
}

bool BrushlessControl::getDirection(void)
{
    RequestMsg_t msg;
    msg.cmd = CMD_ENCODER_GET_DIRECTION;
    return request(msg);
}

float BrushlessControl::getPosition(void)
{
    uint32_t data;
    float position;
    RequestMsg_t msg;
    msg.cmd = CMD_ENCODER_GET_POSITION;
    data = request(msg);
    memcpy(&position, &data, sizeof(float));
    return position;
}

#endif