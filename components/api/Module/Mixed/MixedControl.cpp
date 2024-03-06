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
    Module_RequestMsg_t msg;
    msg.request = DIGITAL_WRITE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)level;
    request(msg);
}

int MixedControl::digitalRead(DigitalInputNum_t num)
{
    Module_RequestMsg_t msg;
    msg.request = DIGITAL_READ;
    msg.param = (uint16_t)num;
    return (int)request(msg);
}

void MixedControl::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
    InterruptMode_t mode, void* arg)
{
    Module_RequestMsg_t msg;
    msg.request = ATTACH_INTERRUPT;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[num] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void MixedControl::detachInterrupt(DigitalInputNum_t num)
{
    Module_RequestMsg_t msg;
    msg.request = DETACH_INTERRUPT;
    msg.param = (uint16_t)num;
    request(msg);
}

int MixedControl::analogRead(AnalogInputNum_t num)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_READ;
    msg.param = (uint16_t)num;
    return request(msg);
}

int MixedControl::analogReadMilliVolts(AnalogInputNum_t num)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_READ_MILLIVOLTS;
    msg.param = (uint16_t)num;
    return request(msg);
}

void MixedControl::analogReadMode(AnalogInputNum_t num, AdcMode_t mode)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_READ_MODE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogReadResolution(AdcResBits_t res)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_READ_RESOLUTION;
    msg.data = (uint32_t)res;
    request(msg);
}

void MixedControl::analogReadReference(float ref)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_READ_REFERENCE;
    memcpy(&msg.data, &ref, sizeof(uint32_t));
    request(msg);
}

void MixedControl::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_OUTPUT_MODE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogWrite(AnalogOutput_Num_t num, float value)
{
    Module_RequestMsg_t msg;
    msg.request = ANALOG_WRITE;
    memcpy(&msg.data, &value, sizeof(float));
    request(msg);
}

#endif