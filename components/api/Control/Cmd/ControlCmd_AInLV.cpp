/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlCmd_AInLV.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ControlCmd_AInLV.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void ControlCmd_AInLV::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_INPUT_MODE, (uint8_t)num, (uint8_t)mode};
    _control->request(msgBytes);
}

uint8_t ControlCmd_AInLV::analogInputGetMode(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_INPUT_GET_MODE, (uint8_t)num};
    _control->request(msgBytes);
    return msgBytes[2];
}

void ControlCmd_AInLV::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_INPUT_VOLTAGE_RANGE, (uint8_t)num, (uint8_t)range};
    _control->request(msgBytes);
}

uint8_t ControlCmd_AInLV::analogInputGetVoltageRange(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_INPUT_GET_VOLTAGE_RANGE, (uint8_t)num};
    _control->request(msgBytes);
    return msgBytes[2];
}

int ControlCmd_AInLV::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ, (uint8_t)num};
    _control->request(msgBytes);
    int* ret = reinterpret_cast<int*>(&msgBytes[2]);
    return *ret;
}

float ControlCmd_AInLV::analogReadVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_VOLT, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float ControlCmd_AInLV::analogReadMilliVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_MILLIVOLT, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float ControlCmd_AInLV::analogReadAmp(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_AMP, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float ControlCmd_AInLV::analogReadMilliAmp(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_MILLIAMP, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif