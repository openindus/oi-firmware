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

#if !defined(OI_BRUSHLESS)

void BrushlessControl::setSpeed(uint32_t duty_cycle)
{
    Module_RequestMsg_t msg;
    msg.request = MOTOR_SET_SPEED;
    msg.data = (uint32_t)duty_cycle;
    request(msg);
}

void BrushlessControl::setBrake(bool brake)
{
    Module_RequestMsg_t msg;
    msg.request = MOTOR_SET_BRAKE;
    msg.data = (bool)brake;
    request(msg);
}

void BrushlessControl::setDirection(bool direction)
{
    Module_RequestMsg_t msg;
    msg.request = MOTOR_SET_DIRECTION;
    msg.data = (bool)direction;
    request(msg);
}

float BrushlessControl::getSpeed(void)
{
    uint32_t data;
    float speed;
    Module_RequestMsg_t msg;
    msg.request = MOTOR_GET_SPEED;
    data = request(msg);
    memcpy(&speed, &data, sizeof(float));
    return speed;
}

float BrushlessControl::getSpeedEncoder(void)
{
    uint32_t data;
    float speed;
    Module_RequestMsg_t msg;
    msg.request = ENCODER_GET_SPEED;
    data = request(msg);
    memcpy(&speed, &data, sizeof(float));
    return speed;
}

bool BrushlessControl::getDirection(void)
{
    Module_RequestMsg_t msg;
    msg.request = ENCODER_GET_DIRECTION;
    return request(msg);
}

float BrushlessControl::getPosition(void)
{
    uint32_t data;
    float position;
    Module_RequestMsg_t msg;
    msg.request = ENCODER_GET_POSITION;
    data = request(msg);
    memcpy(&position, &data, sizeof(float));
    return position;
}

#endif