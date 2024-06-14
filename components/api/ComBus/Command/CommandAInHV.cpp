/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CommandAInHV.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CommandAInHV.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

float CommandAInHV::analogRead(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float CommandAInHV::analogReadVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_VOLT, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

float CommandAInHV::analogReadMilliVolt(AnalogInput_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_READ_MILLIVOLT, (uint8_t)num};
    _control->request(msgBytes);
    float* ret = reinterpret_cast<float*>(&msgBytes[2]);
    return *ret;
}

#endif