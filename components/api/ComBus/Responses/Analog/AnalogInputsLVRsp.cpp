/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLVRsp.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLVRsp.h"

#if defined(MODULE_SLAVE)

int AnalogInputsLVRsp::init() {
    
    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_INPUT_MODE, [](std::vector<uint8_t>& data) {
        AnalogInputsLV::analogInputMode((AnalogInput_Num_t)data[1], (AnalogInput_Mode_t)data[2]);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_INPUT_GET_MODE, [](std::vector<uint8_t>& data) {
        uint8_t mode = AnalogInputsLV::analogInputGetMode((AnalogInput_Num_t)data[1]);
        data.push_back(mode);
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_INPUT_VOLTAGE_RANGE, [](std::vector<uint8_t>& data) {
        AnalogInputsLV::analogInputVoltageRange((AnalogInput_Num_t)data[1], (AnalogInput_VoltageRange_t)data[2]);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_INPUT_GET_VOLTAGE_RANGE, [](std::vector<uint8_t>& data) {
        uint8_t range = AnalogInputsLV::analogInputGetVoltageRange((AnalogInput_Num_t)data[1]);
        data.push_back(range);
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_READ, [](std::vector<uint8_t>& data) {
        int value = AnalogInputsLV::analogRead((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int));
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_READ_VOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLV::analogReadVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLV::analogReadMilliVolt((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_READ_AMP, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLV::analogReadAmp((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    ControllerSlave::addCtrlCallback(REQUEST_ANALOG_READ_MILLIAMP, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLV::analogReadMilliAmp((AnalogInput_Num_t)data[1]);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
    });

    return 0;
}

#endif