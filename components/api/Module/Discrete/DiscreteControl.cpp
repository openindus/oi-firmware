/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteControl.h"

#if !defined(CONFIG_DISCRETE) && !defined(CONFIG_DISCRETE_VE)

void DiscreteControl::digitalWrite(DigitalOutputNum_t stor, uint8_t level)
{
    RequestMsg_t msg;
    msg.cmd = CMD_DIGITAL_WRITE;
    msg.param = (uint16_t)stor;
    msg.data = (uint32_t)level;
    request(msg);
}

void DiscreteControl::analogWrite(DigitalOutputNum_t stor, uint8_t duty)
{
    RequestMsg_t msg;
    msg.cmd = CMD_ANALOG_WRITE;
    msg.param = (uint16_t)stor;
    msg.data = (uint32_t)duty;
    request(msg);
}

int DiscreteControl::analogRead(AnalogInputNum_t eana)
{
    RequestMsg_t msg;
    msg.cmd = CMD_ANALOG_READ;
    msg.param = (uint16_t)eana;
    return request(msg);
}

void DiscreteControl::attachInterrupt(DigitalInputNum_t etor, IsrCallback_t callback, InterruptMode_t mode)
{
    RequestMsg_t msg;
    msg.cmd = CMD_ATTACH_INTERRUPT;
    msg.param = (uint16_t)etor;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[etor] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void DiscreteControl::detachInterrupt(DigitalInputNum_t etor)
{
    RequestMsg_t msg;
    msg.cmd = CMD_DETACH_INTERRUPT;
    msg.param = (uint16_t)etor;
    request(msg);
}

#endif
