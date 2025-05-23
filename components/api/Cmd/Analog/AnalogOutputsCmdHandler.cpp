/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogOutputsCmdHandler.cpp
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogOutputsCmdHandler.h"

#if defined(MODULE_SLAVE)

int AnalogOutputsCmdHandler::init() {
    
    Slave::addCallback(CALLBACK_ANALOG_OUTPUT_MODE, [](std::vector<uint8_t>& data) {
        AnalogOutputs::analogOutputMode((AnalogOutput_Num_t)data[1], (AnalogOutput_Mode_t)data[2]);
        data.clear();
    });

    Slave::addCallback(CALLBACK_ANALOG_WRITE, [](std::vector<uint8_t>& data) {
        float* value = reinterpret_cast<float*>(&data[2]);
        AnalogOutputs::analogWrite((AnalogOutput_Num_t)data[1], *value);
        data.clear();
    });
    
    return 0;
}

#endif