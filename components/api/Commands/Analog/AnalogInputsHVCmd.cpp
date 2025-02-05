/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHVCmd.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsHVCmd.h"
#include "Commands.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

float AnalogInputsHVCmd::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ, (uint8_t)num};
    _control->sendRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsHVCmd::analogReadVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_VOLT, (uint8_t)num};
    _control->sendRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float AnalogInputsHVCmd::analogReadMilliVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_MILLIVOLT, (uint8_t)num};
    _control->sendRequest(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif