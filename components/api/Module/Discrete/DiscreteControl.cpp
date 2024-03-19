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

#if !defined(OI_DISCRETE) && !defined(OI_DISCRETE_VE)

void DiscreteControl::digitalWrite(DigitalOutputNum_t dout, uint8_t level)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DIGITAL_WRITE;
    msg.param = (uint16_t)dout;
    msg.data = (uint32_t)level;
    request(msg);
}

int DiscreteControl::digitalRead(DigitalInputNum_t din)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DIGITAL_READ;
    msg.param = (uint16_t)din;
    return (int)request(msg);
}

void DiscreteControl::analogWrite(DigitalOutputNum_t dout, uint8_t duty)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_WRITE;
    msg.param = (uint16_t)dout;
    msg.data = (uint32_t)duty;
    request(msg);
}

int DiscreteControl::analogRead(AnalogInput_Num_t eana)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_READ;
    msg.param = (uint16_t)eana;
    return request(msg);
}

void DiscreteControl::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ATTACH_INTERRUPT;
    msg.param = (uint16_t)din;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[din] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(EVENT_DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void DiscreteControl::detachInterrupt(DigitalInputNum_t din)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DETACH_INTERRUPT;
    msg.param = (uint16_t)din;
    request(msg);
}

#endif
