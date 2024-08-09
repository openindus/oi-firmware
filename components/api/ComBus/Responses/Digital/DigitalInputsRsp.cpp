/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsRsp.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsRsp.h"

#if defined(MODULE_SLAVE)

IsrCallback_t DigitalInputsRsp::_isrCallback[] = {
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_5});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_6});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_7});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_8});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_9});},
    [](void*){ControllerSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_10});},
};

int DigitalInputsRsp::init() {
    
    ControllerSlave::addCtrlCallback(REQUEST_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = DigitalInputs::digitalRead((DIn_Num_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    ControllerSlave::addCtrlCallback(REQUEST_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]], (InterruptMode_t)data[2]);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::detachInterrupt((DIn_Num_t)data[1]);
        data.clear();
    });
    
    return 0;
}

#endif