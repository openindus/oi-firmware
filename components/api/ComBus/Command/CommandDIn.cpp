/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CommandDIn.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "CommandDIn.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

int CommandDIn::digitalRead(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_READ, (uint8_t)num};
    _control->request(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void CommandDIn::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num] = callback;
    _control->addEventCallback(EVENT_DIGITAL_INTERRUPT, _control->getId(), [this](uint8_t num) { _isrCallback[num](NULL); });
    _control->request(msgBytes);
}

void CommandDIn::detachInterrupt(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DETACH_INTERRUPT, (uint8_t)num};
    _control->request(msgBytes);
}

#endif