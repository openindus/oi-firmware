/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlSlave_DIn.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ControlSlave_DIn.h"

#if defined(MODULE_SLAVE)

IsrCallback_t ControlSlave_DIn::_isrCallback[] = {
    [](void*){ControlSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){ControlSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){ControlSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){ControlSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

int ControlSlave_DIn::init() {
    
    ControlSlave::addCtrlCallback(REQUEST_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = DigitalInputs::digitalRead((DIn_Num_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    ControlSlave::addCtrlCallback(REQUEST_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]], (InterruptMode_t)data[2]);
        data.clear();
    });

    ControlSlave::addCtrlCallback(REQUEST_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::detachInterrupt((DIn_Num_t)data[1]);
        data.clear();
    });
    
    return 0;
}

#endif