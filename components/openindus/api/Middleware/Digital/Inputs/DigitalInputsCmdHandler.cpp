/**
 * @file DigitalInputsCmdHandler.cpp
 * @brief Digital inputs commands handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsCmdHandler.h"

#if defined(CONFIG_MODULE_SLAVE)

IsrCallback_t DigitalInputsCmdHandler::_isrCallback[] = {
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
#if defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DISCRETE_VE)
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_5});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_6});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_7});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_8});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_9});},
    [](void*){Slave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_10});},
#endif
};

#endif