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

#if !defined(CONFIG_MIXED)

void MixedControl::digitalWrite(DigitalOutputNum_t dout, uint8_t level)
{
    RequestMsg_t msg;
    msg.request = DIGITAL_WRITE;
    msg.param = (uint16_t)dout;
    msg.data = (uint32_t)level;
    request(msg);
}

int MixedControl::digitalRead(DigitalInputNum_t din)
{
    RequestMsg_t msg;
    msg.request = DIGITAL_READ;
    msg.param = (uint16_t)din;
    return (int)request(msg);
}

void MixedControl::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode)
{
    RequestMsg_t msg;
    msg.request = ATTACH_INTERRUPT;
    msg.param = (uint16_t)din;
    msg.data = (uint32_t)mode;
    request(msg);
    _isrCallback[din] = callback;
    uint16_t id = ModuleControl::getId(this);
    ModuleMaster::onEvent(DIGITAL_INTERRUPT, id, [this](uint8_t num) {
        _isrCallback[num](NULL);
    });
}

void MixedControl::detachInterrupt(DigitalInputNum_t din)
{
    RequestMsg_t msg;
    msg.request = DETACH_INTERRUPT;
    msg.param = (uint16_t)din;
    request(msg);
}

int MixedControl::analogRead(AnalogInputNum_t ain)
{
    RequestMsg_t msg;
    msg.request = ANALOG_READ;
    msg.param = (uint16_t)ain;
    return request(msg);
}

int MixedControl::analogReadMilliVolts(AnalogInputNum_t ain)
{
    RequestMsg_t msg;
    msg.request = ANALOG_READ_MILLIVOLTS;
    msg.param = (uint16_t)ain;
    return request(msg);
}

void MixedControl::analogReadMode(AnalogInputNum_t ain, AdcMode_t mode)
{
    RequestMsg_t msg;
    msg.request = ANALOG_READ_MODE;
    msg.param = (uint16_t)ain;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogReadResolution(AdcResBits_t res)
{
    RequestMsg_t msg;
    msg.request = ANALOG_READ_RESOLUTION;
    msg.data = (uint32_t)res;
    request(msg);
}

void MixedControl::analogReadReference(float ref)
{
    RequestMsg_t msg;
    msg.request = ANALOG_READ_REFERENCE;
    memcpy(&msg.data, &ref, sizeof(uint32_t));
    request(msg);
}

void MixedControl::analogWriteVoltage(AnalogOutputNum_t aout, uint32_t value)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_VOLTAGE;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)value;
    request(msg);
}

void MixedControl::analogWriteVoltageMilliVolts(AnalogOutputNum_t aout, uint32_t value)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_VOLTAGE_MILLIVOLTS;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)value;
    request(msg);
}

void MixedControl::analogWriteVoltageMode(AnalogOutputNum_t aout, DacVoltageMode_t mode)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_VOLTAGE_MODE;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogWriteCurrent(AnalogOutputNum_t aout, uint32_t value)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_CURRENT;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)value;
    request(msg);
}

void MixedControl::analogWriteCurrentMilliAmps(AnalogOutputNum_t aout, uint32_t value)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_CURRENT_MILLIAMPS;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)value;
    request(msg);
}

void MixedControl::analogWriteCurrentMode(AnalogOutputNum_t aout, DacCurrentMode_t mode)
{
    RequestMsg_t msg;
    msg.request = ANALOG_WRITE_CURRENT_MODE;
    msg.param = (uint16_t)aout;
    msg.data = (uint32_t)mode;
    request(msg);
}

#endif