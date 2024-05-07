/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsSlave.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsSlave.h"

#if defined(MODULE_SLAVE)

IsrCallback_t DigitalInputsSlave::_isrCallback[] = {
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

int DigitalInputsSlave::init() {
    
    ModuleSlave::addCtrlCallback(CONTROL_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = DigitalInputs::digitalRead((DigitalInputNum_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    ModuleSlave::addCtrlCallback(CONTROL_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::attachInterrupt((DigitalInputNum_t)data[1], _isrCallback[data[1]], (InterruptMode_t)data[2]);
        data.clear();
    });

    ModuleSlave::addCtrlCallback(CONTROL_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::detachInterrupt((DigitalInputNum_t)data[1]);
        data.clear();
    });
    
    return 0;
}

#endif