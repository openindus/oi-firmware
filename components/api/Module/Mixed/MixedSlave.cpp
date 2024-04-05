/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedSlave.cpp
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MixedSlave.h"

#if defined(OI_MIXED)

IsrCallback_t MixedSlave::_isrCallback[] = {
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

MixedStandalone* MixedSlave::_mixed = new MixedStandalone();

int MixedSlave::init(void)
{
    ModuleSlave::init();

    addCtrlCallback(CONTROL_DIGITAL_WRITE, [](std::vector<uint8_t>& data) {
        _mixed->digitalWrite((DigitalOutputNum_t)data[1], data[2]);
    });

    addCtrlCallback(CONTROL_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = _mixed->digitalRead((DigitalInputNum_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    addCtrlCallback(CONTROL_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        _mixed->attachInterrupt((DigitalInputNum_t)data[1], 
            _isrCallback[data[1]], (InterruptMode_t)data[2]);
    });

    addCtrlCallback(CONTROL_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        _mixed->detachInterrupt((DigitalInputNum_t)data[1]);
    });

    onRequest(REQUEST_ANALOG_READ, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        return _mixed->analogRead((AnalogInput_Num_t)msg.param);
    });

    onRequest(REQUEST_ANALOG_READ_MILLIVOLT, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        return _mixed->analogReadMilliVolt((AnalogInput_Num_t)msg.param);
    });

    addCtrlCallback(CONTROL_ANALOG_INPUT_MODE, [](std::vector<uint8_t>& data) {
        _mixed->analogInputMode((AnalogInput_Num_t)data[1], (AnalogInput_Mode_t)data[2]);
    });

    addCtrlCallback(CONTROL_ANALOG_OUTPUT_MODE, [](std::vector<uint8_t>& data) {
        _mixed->analogOutputMode((AnalogOutput_Num_t)data[1], (AnalogOutput_Mode_t)data[2]);
    });

    addCtrlCallback(CONTROL_ANALOG_WRITE, [](std::vector<uint8_t>& data) {
        float* value = reinterpret_cast<float*>(&data[2]);
        _mixed->analogWrite((AnalogOutput_Num_t)data[1], *value);
    });

    return 0;
}

#endif
