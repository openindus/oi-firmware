/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AanalogOutputsControl.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogOutputsControl.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void AnalogOutputsControl::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    ModuleControl::ctrlRequest(msgBytes);
}

void AnalogOutputsControl::analogWrite(AnalogOutput_Num_t num, float value)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_WRITE};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ModuleControl::ctrlRequest(msgBytes);
}

#endif