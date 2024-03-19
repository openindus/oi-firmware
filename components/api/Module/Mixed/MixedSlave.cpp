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
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(EVENT_DIGITAL_INTERRUPT, (int)DIN_4);},
};

MixedStandalone* MixedSlave::_mixed = new MixedStandalone();

int MixedSlave::init(void)
{
    ModuleSlave::init();

    onRequest(REQUEST_DIGITAL_WRITE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        _mixed->digitalWrite((DigitalOutputNum_t)msg.param, (uint8_t)msg.data);
        return 0;
    });

    onRequest(REQUEST_DIGITAL_READ, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        return _mixed->digitalRead((DigitalInputNum_t)msg.param);
    });

    onRequest(REQUEST_ATTACH_INTERRUPT, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        _mixed->attachInterrupt(din, _isrCallback[din], mode); 
        return 0;
    });

    onRequest(REQUEST_DETACH_INTERRUPT, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        _mixed->detachInterrupt(din);
        return 0;
    });

    onRequest(REQUEST_ANALOG_READ, [](ModuleCmd_RequestMsg_t msg) -> uint32_t {
        return _mixed->analogRead((AnalogInput_Num_t)msg.param);
    });

    onRequest(REQUEST_ANALOG_READ_MILLIVOLTS, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        return _mixed->analogReadMilliVolts((AnalogInput_Num_t)msg.param);
    });

    onRequest(REQUEST_ANALOG_INPUT_MODE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        _mixed->analogInputMode((AnalogInput_Num_t)msg.param, (AnalogInput_Mode_t)msg.data);
        return 0;
    });

    onRequest(REQUEST_ANALOG_INPUT_RESOLUTION, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        _mixed->analogInputResolution((AnalogInput_Resolution_t)msg.data);
        return 0;
    });

    onRequest(REQUEST_ANALOG_INPUT_REFERENCE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        float value;
        uint32_t data = msg.data;
        memcpy(&value, &data, sizeof(float));
        _mixed->analogInputReference(value);
        return 0;
    });

    onRequest(REQUEST_ANALOG_OUTPUT_MODE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        _mixed->analogOutputMode((AnalogOutput_Num_t)msg.param, (AnalogOutput_Mode_t)msg.data);
        return 0;
    });

    onRequest(REQUEST_ANALOG_WRITE, [](ModuleCmd_RequestMsg_t msg) -> uint32_t { 
        float value;
        uint32_t data = msg.data;
        memcpy(&value, &data, sizeof(float));
        _mixed->analogWrite((AnalogOutput_Num_t)msg.param, value);
        return 0;
    });

    return 0;
}

#endif
