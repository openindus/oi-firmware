/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHVSlave.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsHVSlave.h"

#if defined(MODULE_SLAVE)

int AnalogInputsHVSlave::init() {
    
    ModuleSlave::addCtrlCallback(CONTROL_ANALOG_READ, [](std::vector<uint8_t>& data) {
        int value = AnalogInputsHV::analogRead((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int));
    });

    ModuleSlave::addCtrlCallback(CONTROL_ANALOG_READ_VOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsHV::analogReadVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    ModuleSlave::addCtrlCallback(CONTROL_ANALOG_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsHV::analogReadMilliVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });
    return 0;
}

#endif