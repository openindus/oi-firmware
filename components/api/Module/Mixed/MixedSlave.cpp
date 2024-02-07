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

#if defined(CONFIG_MIXED)

IsrCallback_t MixedSlave::_isrCallback[] = {
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_4);},
};

void MixedSlave::init(void)
{
    MixedStandalone::init();
    ModuleSlave::init();

    onRequest(DIGITAL_WRITE, [](RequestMsg_t msg) -> uint32_t {
        MixedStandalone::digitalWrite((DigitalOutputNum_t)msg.param, (uint8_t)msg.data);
        return 0;
    });

    onRequest(DIGITAL_READ, [](RequestMsg_t msg) -> uint32_t {
        return MixedStandalone::digitalRead((DigitalInputNum_t)msg.param);
    });

    onRequest(ATTACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        MixedStandalone::attachInterrupt(din, _isrCallback[din], mode); 
        return 0;
    });

    onRequest(DETACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        MixedStandalone::detachInterrupt(din);
        return 0;
    });

    onRequest(ANALOG_READ, [](RequestMsg_t msg) -> uint32_t {
        return MixedStandalone::analogRead((AnalogInputNum_t)msg.param);
    });

    onRequest(ANALOG_READ_MILLIVOLTS, [](RequestMsg_t msg) -> uint32_t { 
        return MixedStandalone::analogReadMilliVolts((AnalogInputNum_t)msg.param);
    });

    onRequest(ANALOG_READ_MODE, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogReadMode((AnalogInputNum_t)msg.param, (AdcMode_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_READ_RESOLUTION, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogReadResolution((AdcResBits_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_READ_REFERENCE, [](RequestMsg_t msg) -> uint32_t { 
        float value;
        uint32_t data = msg.data;
        memcpy(&value, &data, sizeof(float));
        MixedStandalone::analogReadReference(value);
        return 0;
    });

    onRequest(ANALOG_WRITE_VOLTAGE, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteVoltage((AnalogOutputNum_t)msg.param, msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE_VOLTAGE_MILLIVOLTS, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteVoltageMilliVolts((AnalogOutputNum_t)msg.param, msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE_VOLTAGE_MODE, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteVoltageMode((AnalogOutputNum_t)msg.param, (DacVoltageMode_t)msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE_CURRENT, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteCurrent((AnalogOutputNum_t)msg.param, msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE_CURRENT_MILLIAMPS, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteCurrentMilliAmps((AnalogOutputNum_t)msg.param, msg.data);
        return 0;
    });

    onRequest(ANALOG_WRITE_CURRENT_MODE, [](RequestMsg_t msg) -> uint32_t { 
        MixedStandalone::analogWriteCurrentMode((AnalogOutputNum_t)msg.param, (DacCurrentMode_t)msg.data);
        return 0;
    });
}

#endif
