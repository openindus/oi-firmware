/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLVControl.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLVControl.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void AnalogInputsLVControl::analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_INPUT_MODE, (uint8_t)num, (uint8_t)mode};
    ModuleControl::ctrlRequest(msgBytes);
}

uint8_t AnalogInputsLVControl::analogInputGetMode(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_INPUT_GET_MODE, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    return msgBytes[2];
}

void AnalogInputsLVControl::analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_INPUT_VOLTAGE_RANGE, (uint8_t)num, (uint8_t)range};
    ModuleControl::ctrlRequest(msgBytes);
}

uint8_t AnalogInputsLVControl::analogInputGetVoltageRange(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_INPUT_GET_VOLTAGE_RANGE, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    return msgBytes[2];
}

int AnalogInputsLVControl::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    int* ret = reinterpret_cast<int*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsLVControl::analogReadVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_VOLT, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsLVControl::analogReadMilliVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_MILLIVOLT, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsLVControl::analogReadAmp(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_AMP, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsLVControl::analogReadMilliAmp(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_MILLIAMP, (uint8_t)num};
    ModuleControl::ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif