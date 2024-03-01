/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteSlave.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteSlave.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

IsrCallback_t DiscreteSlave::_isrCallback[] = {
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_1);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_2);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_3);},
    [](void*){event(DIGITAL_INTERRUPT, (int)DIN_4);},
};

void DiscreteSlave::init(void)
{    
    DiscreteStandalone::init();
    ModuleSlave::init();
    
    onRequest(DIGITAL_WRITE, [](RequestMsg_t msg) -> uint32_t {
        DigitalOutputNum_t dout = (DigitalOutputNum_t)msg.param;
        uint8_t level = (uint8_t)msg.data;
        DiscreteStandalone::digitalWrite(dout, level);
        return 0;
    });

    onRequest(ANALOG_WRITE, [](RequestMsg_t msg) -> uint32_t {
        DigitalOutputNum_t dout = (DigitalOutputNum_t)msg.param;
        uint8_t duty = (uint8_t)msg.data;
        DiscreteStandalone::analogWrite(dout, duty);
        return 0;
    });

    onRequest(DIGITAL_READ, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        return DiscreteStandalone::digitalRead(din);
    });

    onRequest(ANALOG_READ, [](RequestMsg_t msg) -> uint32_t {
        AnalogInputNum_t ain = (AnalogInputNum_t)msg.param;
        return DiscreteStandalone::analogRead(ain);
    });

    onRequest(ATTACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        DiscreteStandalone::attachInterrupt(din, _isrCallback[din], mode);
        return 0;
    });

    onRequest(DETACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t din = (DigitalInputNum_t)msg.param;
        DiscreteStandalone::detachInterrupt(din);
        return 0;
    });
}

#endif
