/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHVControl.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsHVControl.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

int AnalogInputsHVControl::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
    int* ret = reinterpret_cast<int*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsHVControl::analogReadVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_VOLT, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsHVControl::analogReadMilliVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ_MILLIVOLT, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif