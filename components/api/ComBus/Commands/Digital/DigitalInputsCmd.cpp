/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsCmd.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalInputsCmd.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

DIn_Num_t dinNumTable[DIN_MAX] = {DIN_1, DIN_2, DIN_3, DIN_4, DIN_5, DIN_6, DIN_7, DIN_8, DIN_9, DIN_10};

int DigitalInputsCmd::digitalRead(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_READ, (uint8_t)num};
    _control->request(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DigitalInputsCmd::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num] = callback;
    ControllerMaster::addEventCallback(EVENT_DIGITAL_INTERRUPT, _control->getId(), [this](uint8_t num) { _isrCallback[num](&dinNumTable[num]); });
    _control->request(msgBytes);
}

void DigitalInputsCmd::detachInterrupt(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DETACH_INTERRUPT, (uint8_t)num};
    _control->request(msgBytes);
}

#endif