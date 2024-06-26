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
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_1});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_2});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_3});},
    [](void*){ModuleSlave::sendEvent({EVENT_DIGITAL_INTERRUPT, DIN_4});},
};

DiscreteStandalone* DiscreteSlave::_discrete = new DiscreteStandalone();

int DiscreteSlave::init(void)
{
    ModuleSlave::init();

    addCtrlCallback(CONTROL_DIGITAL_WRITE, [](std::vector<uint8_t>& data) { 
        _discrete->digitalWrite((DigitalOutputNum_t)data[1], data[2]);
    });

    addCtrlCallback(CONTROL_DIGITAL_READ, [](std::vector<uint8_t>& data) { 
        int level = _discrete->digitalRead((DigitalInputNum_t)data[1]);
        data.push_back(static_cast<uint8_t>(level));
    });

    addCtrlCallback(CONTROL_ANALOG_READ, [](std::vector<uint8_t>& data) { 
        int value = _discrete->analogRead((AnalogInput_Num_t)data[1]);
        uint8_t *ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int));
    });

    addCtrlCallback(CONTROL_ANALOG_WRITE, [](std::vector<uint8_t>& data) { 
        _discrete->analogWrite((DigitalOutputNum_t)data[1], data[2]);
    });

    addCtrlCallback(CONTROL_ATTACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        _discrete->attachInterrupt((DigitalInputNum_t)data[1], 
            _isrCallback[data[1]], (InterruptMode_t)data[2]);
    });

    addCtrlCallback(CONTROL_DETACH_INTERRUPT, [](std::vector<uint8_t>& data) { 
        _discrete->detachInterrupt((DigitalInputNum_t)data[1]);
    });

    return 0;
}

#endif
