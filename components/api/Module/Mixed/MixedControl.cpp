/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedControl.cpp
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedControl.h"

#if !defined(OI_MIXED)

void MixedControl::digitalWrite(DigitalOutputNum_t num, uint8_t level)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DIGITAL_WRITE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)level;
    request(msg);
}

int MixedControl::digitalRead(DigitalInputNum_t num)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DIGITAL_READ;
    msg.param = (uint16_t)num;
    return (int)request(msg);
}

void MixedControl::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
    InterruptMode_t mode, void* arg)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ATTACH_INTERRUPT;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[num] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(EVENT_DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void MixedControl::detachInterrupt(DigitalInputNum_t num)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_DETACH_INTERRUPT;
    msg.param = (uint16_t)num;
    request(msg);
}

void MixedControl::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_INPUT_MODE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogInputResolution(AnalogInput_Resolution_t res)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_INPUT_RESOLUTION;
    msg.data = (uint32_t)res;
    request(msg);
}

void MixedControl::analogInputReference(float ref)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_INPUT_REFERENCE;
    memcpy(&msg.data, &ref, sizeof(uint32_t));
    request(msg);
}

int MixedControl::analogRead(AnalogInput_Num_t num)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_READ;
    msg.param = (uint16_t)num;
    return request(msg);
}

float MixedControl::analogReadMilliVolts(AnalogInput_Num_t num)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_READ_MILLIVOLTS;
    msg.param = (uint16_t)num;
    return request(msg);
}

void MixedControl::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_OUTPUT_MODE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogWrite(AnalogOutput_Num_t num, float value)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_WRITE;
    memcpy(&msg.data, &value, sizeof(float));
    request(msg);
}

#endif