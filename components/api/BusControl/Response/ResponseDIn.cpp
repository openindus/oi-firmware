/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ResponseDIn.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ResponseDIn.h"

#if defined(MODULE_SLAVE)

IsrCallback_t ResponseDIn::_isrCallback[] = {
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

int ResponseDIn::init() {
    
    Slave::addCtrlCallback(REQUEST_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = DigitalInputs::digitalRead((DIn_Num_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    Slave::addCtrlCallback(REQUEST_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]], (InterruptMode_t)data[2]);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::detachInterrupt((DIn_Num_t)data[1]);
        data.clear();
    });
    
    return 0;
}

#endif