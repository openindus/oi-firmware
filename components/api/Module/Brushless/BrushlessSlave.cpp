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

#if defined(OI_BRUSHLESS) && defined(MODULE_SLAVE)

void BrushlessSlave::init(void)
{
    ModuleSlave::init();
    BrushLessStandalone::init();

    addCtrlCallback(CONTROL_MOTOR_SET_SPEED, [](std::vector<uint8_t>& data) {
        uint32_t* duty_cycle = reinterpret_cast<uint32_t*>(&data[2]);
        BrushlessStandalone::setSpeed(*duty_cycle);
    });

    addCtrlCallback(CONTROL_MOTOR_SET_BRAKE, [](std::vector<uint8_t>& data) {
        bool brake = static_cast<bool>(data[2]);
        BrushlessStandalone::setBrake(brake);
    });

    addCtrlCallback(CONTROL_MOTOR_SET_DIRECTION, [](std::vector<uint8_t>& data) {
        bool direction = static_cast<bool>(data[2]);
        BrushlessStandalone::setDirection(direction);
    });

    addCtrlCallback(CONTROL_MOTOR_GET_SPEED, [](std::vector<uint8_t>& data) {
        float speed = BrushlessStandalone::getSpeed(); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    addCtrlCallback(CONTROL_ENCODER_GET_SPEED, [](std::vector<uint8_t>& data) {
        float speed = BrushlessStandalone::getSpeedEncoder(); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&speed);
        data.insert(data.end(), ptr, ptr + sizeof(float)); 
    });

    addCtrlCallback(CONTROL_ENCODER_GET_DIRECTION, [](std::vector<uint8_t>& data) {
        bool direction = BrushlessStandalone::getDirection();
        data.push_back(static_cast<uint8_t>(direction));
    });

    addCtrlCallback(CONTROL_ENCODER_GET_POSITION, [](std::vector<uint8_t>& data) {
        float position = BrushlessStandalone::getPosition(); 
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&position);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });
}

#endif