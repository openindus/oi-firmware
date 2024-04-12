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
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_SPEED};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&duty_cycle);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(uint32_t));
    ctrlRequest(msgBytes);
}

void BrushlessControl::setBrake(bool brake)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_BRAKE, (uint8_t)brake};
    ctrlRequest(msgBytes);
}

void BrushlessControl::setDirection(bool direction)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_SET_DIRECTION, (uint8_t)direction};
    ctrlRequest(msgBytes);
}

float BrushlessControl::getSpeed(void)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_SPEED};
    ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float BrushlessControl::getSpeedEncoder(void)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ENCODER_GET_SPEED};
    ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

bool BrushlessControl::getDirection(void)
{
    std::vector<uint8_t> msgBytes = {CONTROL_MOTOR_GET_SPEED};
    ctrlRequest(msgBytes);
    return static_cast<bool>(&msgBytes[2]);
}

float BrushlessControl::getPosition(void)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ENCODER_GET_POSITION};
    ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif