/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ResponseDOut.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ResponseDOut.h"

#if defined(MODULE_SLAVE)

int ResponseDOut::init() {
    
    Slave::addCtrlCallback(REQUEST_DIGITAL_WRITE, [](std::vector<uint8_t>& data) {
        DigitalOutputs::digitalWrite((DOut_Num_t)data[1], data[2]);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_TOGGLE_OUTPUT, [](std::vector<uint8_t>& data) {
        DigitalOutputs::toggleOutput((DOut_Num_t)data[1]);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_OUTPUT_MODE, [](std::vector<uint8_t>& data) {
        DigitalOutputs::outputMode((DOut_Num_t)data[1], (DOut_Mode_t)data[2]);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_SET_PWM_FREQUENCY, [](std::vector<uint8_t>& data) {
        uint32_t* freq = reinterpret_cast<uint32_t*>(&data[2]);
        DigitalOutputs::setPWMFrequency((DOut_Num_t)data[1], *freq);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_SET_PWM_DUTY_CYCLE, [](std::vector<uint8_t>& data) {
        uint32_t* duty = reinterpret_cast<uint32_t*>(&data[2]);
        DigitalOutputs::setPWMDutyCycle((DOut_Num_t)data[1], *duty);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_GET_OUTPUT_CURRENT, [](std::vector<uint8_t>& data) {
        float current = DigitalOutputs::getOutputCurrent((DOut_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&current);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });
    
    return 0;
}

#endif