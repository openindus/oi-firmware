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
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    ctrlRequest(msgBytes);
}

int MixedControl::digitalRead(DigitalInputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_READ, (uint8_t)num};
    ctrlRequest(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void MixedControl::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
    InterruptMode_t mode, void* arg)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num] = callback;
    ctrlRequest(msgBytes);
    
    addEventCallback(
        EVENT_DIGITAL_INTERRUPT, 
        _id, 
        [this](uint8_t num) { _isrCallback[num](NULL); }
    );
}

void MixedControl::detachInterrupt(DigitalInputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DETACH_INTERRUPT, (uint8_t)num};
    ctrlRequest(msgBytes);
}

void MixedControl::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_INPUT_MODE, (uint8_t)num, (uint8_t)mode};
    ctrlRequest(msgBytes);
}

void MixedControl::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_INPUT_MODE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)mode;
    request(msg);
}

void MixedControl::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    ModuleCmd_RequestMsg_t msg;
    msg.id = REQUEST_ANALOG_INPUT_VOLTAGE_RANGE;
    msg.param = (uint16_t)num;
    msg.data = (uint32_t)range;
    request(msg);
}

int MixedControl::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ, (uint8_t)num};
    ctrlRequest(msgBytes);
    int* ret = reinterpret_cast<int*>(&msgBytes[2]);
    return *ret;
}

float MixedControl::analogReadMilliVolts(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_MILLIVOLTS, (uint8_t)num};
    ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float MixedControl::analogReadMilliAmps(AnalogInput_Num_t num)
{
    return 0.0;
}

void MixedControl::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    ctrlRequest(msgBytes);
}

void MixedControl::analogWrite(AnalogOutput_Num_t num, float value)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_WRITE};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ctrlRequest(msgBytes);
}

#endif