/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputsSlave.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsSlave.h"

#if defined(MODULE_SLAVE)

int DigitalOutputsSlave::init() {
    
    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_WRITE, [](std::vector<uint8_t>& data) {
        DigitalOutputs::digitalWrite((DigitalOutputNum_t)data[1], data[2]);
    });

    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_TOGGLE, [](std::vector<uint8_t>& data) {
        DigitalOutputs::digitalToggle((DigitalOutputNum_t)data[1]);
    });

    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_MODE_PWM, [](std::vector<uint8_t>& data) {
        uint32_t* freq = reinterpret_cast<uint32_t*>(&data[2]);
        DigitalOutputs::digitalModePWM((DigitalOutputNum_t)data[1], freq);
    });

    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_SET_PWM, [](std::vector<uint8_t>& data) {
        uint32_t* duty = reinterpret_cast<uint32_t*>(&data[2]);
        DigitalOutputs::digitalModePWM((DigitalOutputNum_t)data[1], duty);
    });

    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_GET_CURRENT, [](std::vector<uint8_t>& data) {
        float current = DigitalOutputs::digitalGetCurrent((DigitalOutputNum_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&current);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });
    
    return 0;
}

#endif