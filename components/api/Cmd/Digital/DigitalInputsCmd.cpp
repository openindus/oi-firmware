/**
 * @file DigitalInputsCmd.cpp
 * @brief Digital Inputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalInputsCmd.h"

#if defined(MODULE_MASTER)

DIn_Num_t dinNumTable[DIN_MAX] = {DIN_1, DIN_2, DIN_3, DIN_4, DIN_5,
                                  DIN_6, DIN_7, DIN_8, DIN_9, DIN_10};

int DigitalInputsCmd::digitalRead(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_DIGITAL_READ, (uint8_t)num};
    _module->runCallback(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DigitalInputsCmd::attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_ATTACH_INTERRUPT, (uint8_t)num, (uint8_t)mode};
    _isrCallback[num]             = callback;
    Master::addEventCallback(EVENT_DIGITAL_INTERRUPT, _module->getId(), [this](uint8_t *data) {
        if (_isrCallback[data[1]] != NULL) _isrCallback[data[1]](&dinNumTable[data[1]]);
    });
    _module->runCallback(msgBytes);
}

void DigitalInputsCmd::detachInterrupt(DIn_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_DETACH_INTERRUPT, (uint8_t)num};
    _module->runCallback(msgBytes);
}

#endif