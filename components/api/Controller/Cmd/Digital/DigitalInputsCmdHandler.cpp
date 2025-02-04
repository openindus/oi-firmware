/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsCmdHandler.cpp
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsCmdHandler.h"

#if defined(MODULE_SLAVE)

IsrCallback_t DigitalInputsCmdHandler::_isrCallback[] = {
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_5});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_6});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_7});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_8});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_9});},
    [](void*){SlaveController::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_10});},
};

int DigitalInputsCmdHandler::init() {
    
    SlaveController::addCmdHandler(REQUEST_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = DigitalInputs::digitalRead((DIn_Num_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    SlaveController::addCmdHandler(REQUEST_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]], (InterruptMode_t)data[2]);
        data.clear();
    });

    SlaveController::addCmdHandler(REQUEST_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        DigitalInputs::detachInterrupt((DIn_Num_t)data[1]);
        data.clear();
    });
    
    return 0;
}

#endif