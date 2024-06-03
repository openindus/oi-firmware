/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlCmd_AOut.cpp
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ControlCmd_AOut.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

int ControlCmd_AOut::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    return _control->request(msgBytes);
}

int ControlCmd_AOut::analogWrite(AnalogOutput_Num_t num, float value)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ANALOG_WRITE, (uint8_t)num};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    return _control->request(msgBytes);
}

#endif