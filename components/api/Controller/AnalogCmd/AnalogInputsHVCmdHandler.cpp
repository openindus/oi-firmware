/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsHVCmdHandler.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsHVCmdHandler.h"

#if defined(MODULE_SLAVE)

int AnalogInputsHVCmdHandler::init() {
    
    Slave::addRequestProcessCallback(REQUEST_ANALOG_READ, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsHV::analogRead((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    Slave::addRequestProcessCallback(REQUEST_ANALOG_READ_VOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsHV::analogReadVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    Slave::addRequestProcessCallback(REQUEST_ANALOG_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsHV::analogReadMilliVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });
    return 0;
}

#endif