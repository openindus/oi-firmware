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
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_4);},
};

MixedStandalone* MixedSlave::_mixed = new MixedStandalone();

int MixedSlave::init(void)
{
    ModuleSlave::init();

    onRequest(DIGITAL_WRITE, [](RequestMsg_t msg) -> uint32_t {
        _mixed->digitalWrite((DigitalOutputNum_t)msg.param, (uint8_t)msg.data);
        return 0;
    });

    onRequest(DIGITAL_READ, [](RequestMsg_t msg) -> uint32_t {
        return _mixed->digitalRead((DigitalInputNum_t)msg.param);
    });

    onRequest(ATTACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        _mixed->attachInterrupt(din, _isrCallback[din], mode); 
        return 0;
    });

    onRequest(DETACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        _mixed->detachInterrupt(din);
        return 0;
    });

    onRequest(ANALOG_READ, [](RequestMsg_t msg) -> uint32_t {
        return _mixed->analogRead((AnalogInputNum_t)msg.param);
    });

    onRequest(ANALOG_READ_MILLIVOLTS, [](RequestMsg_t msg) -> uint32_t { 
        return _mixed->analogReadMilliVolts((AnalogInputNum_t)msg.param);
    });

    onRequest(ANALOG_READ_MODE, [](RequestMsg_t msg) -> uint32_t { 
        _mixed->analogReadMode((AnalogInputNum_t)msg.param, (AdcMode_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_READ_RESOLUTION, [](RequestMsg_t msg) -> uint32_t { 
        _mixed->analogReadResolution((AdcResBits_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_READ_REFERENCE, [](RequestMsg_t msg) -> uint32_t { 
        float value;
        uint32_t data = msg.data;
        memcpy(&value, &data, sizeof(float));
        _mixed->analogReadReference(value);
        return 0;
    });

    onRequest(ANALOG_OUTPUT_MODE, [](RequestMsg_t msg) -> uint32_t { 
        _mixed->analogOutputMode((AnalogOutput_Num_t)msg.param, (AnalogOutput_Mode_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE, [](RequestMsg_t msg) -> uint32_t { 
        float value;
        uint32_t data = msg.data;
        memcpy(&value, &data, sizeof(float));
        _mixed->analogWrite((AnalogOutput_Num_t)msg.param, value);
        return 0;
    });

    return 0;
}

#endif
