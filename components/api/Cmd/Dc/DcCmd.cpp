/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcCmd.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#if defined(MODULE_MASTER)

#include "DcCmd.h"

DcCmd::DcCmd(ModuleControl* module) : _module(module) 
{

}

void DcCmd::run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_RUN, (uint8_t)motor, (uint8_t)direction};
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&dutyCycle);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void DcCmd::stop(MotorNum_t motor)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_MOTOR_DC_STOP, (uint8_t)motor};
    _module->runCallback(msgBytes);
}

#endif