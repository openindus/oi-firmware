/**
 * @file DigitalOutputsCmdHandler.cpp
 * @brief Digital outputs commands handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCmdHandler.h"

#if defined(CONFIG_MODULE_SLAVE)

void (*DigitalOutputsCmdHandler::_overcurrentCallback)(void*) = [](void*) {
    Slave::sendEvent({EVENT_OVERCURRENT});
};

#endif