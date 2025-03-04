/**
 * @file DigitalInputsCmd.cpp
 * @brief Digital Inputs Cmd
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsCmd.h"

#if defined(MODULE_MASTER)

DIn_Num_t dinNumTable[DIN_MAX] = {DIN_1, DIN_2, DIN_3, DIN_4, DIN_5,
                                  DIN_6, DIN_7, DIN_8, DIN_9, DIN_10};

int DigitalInputsCmd::digitalRead(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_READ, (uint8_t)num};
    _controllerInst->performRequest(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DigitalInputsCmd::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num]             = callback;
    Master::addEventProcessCallback(
        EVENT_DIGITAL_INTERRUPT, _controllerInst->getId(),
        [this](uint8_t *data) { _isrCallback[data[1]](&dinNumTable[data[1]]); });
    _controllerInst->performRequest(msgBytes);
}

void DigitalInputsCmd::detachInterrupt(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DETACH_INTERRUPT, (uint8_t)num};
    _controllerInst->performRequest(msgBytes);
}

#endif