/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsControl.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsControl.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

int DigitalInputsControl::digitalRead(DigitalInputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_READ, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DigitalInputsControl::attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num] = callback;
    _control->ctrlRequest(msgBytes);
    _control->addEventCallback(_control, EVENT_DIGITAL_INTERRUPT, [this](uint8_t num) { _isrCallback[num](NULL); });
}

void DigitalInputsControl::detachInterrupt(DigitalInputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DETACH_INTERRUPT, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
}

#endif