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

#if defined(CONFIG_DISCRETE) || defined(CONFIG_DISCRETE_VE)

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
    
    onRequest(CMD_DIGITAL_WRITE, [](RequestMsg_t msg) -> uint32_t {
        DigitalOutputNum_t stor = (DigitalOutputNum_t)msg.param;
        uint8_t level = (uint8_t)msg.data;
        DiscreteStandalone::digitalWrite(stor, level);
        return 0;
    });

    onRequest(CMD_ANALOG_WRITE, [](RequestMsg_t msg) -> uint32_t {
        DigitalOutputNum_t stor = (DigitalOutputNum_t)msg.param;
        uint8_t duty = (uint8_t)msg.data;
        DiscreteStandalone::analogWrite(stor, duty);
        return 0;
    });

    onRequest(CMD_DIGITAL_READ, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t etor = (DigitalInputNum_t)msg.param;
        return DiscreteStandalone::digitalRead(etor);
    });

    onRequest(CMD_ANALOG_READ, [](RequestMsg_t msg) -> uint32_t {
        AnalogInputNum_t eana = (AnalogInputNum_t)msg.param;
        return DiscreteStandalone::analogRead(eana);
    });

    onRequest(CMD_ATTACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t etor = (DigitalInputNum_t)msg.param;
        InterruptMode_t mode = (InterruptMode_t)msg.data;
        DiscreteStandalone::attachInterrupt(etor, _isrCallback[etor], mode); 
        return 0;
    });

    onRequest(CMD_DETACH_INTERRUPT, [](RequestMsg_t msg) -> uint32_t {
        DigitalInputNum_t etor = (DigitalInputNum_t)msg.param;
        DiscreteStandalone::detachInterrupt(etor); 
        return 0;
    });
}

#endif
