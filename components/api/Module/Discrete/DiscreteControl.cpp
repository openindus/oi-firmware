/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DiscreteControl.h"

#if !defined(OI_DISCRETE) && !defined(OI_DISCRETE_VE)

void DiscreteControl::digitalWrite(DigitalOutputNum_t dout, uint8_t level)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_WRITE, (uint8_t)dout, level};
    ctrlRequest(msgBytes);
}

int DiscreteControl::digitalRead(DigitalInputNum_t din)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_READ, (uint8_t)din};
    ctrlRequest(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DiscreteControl::analogWrite(DigitalOutputNum_t dout, uint8_t duty)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_WRITE, (uint8_t)dout, duty};
    ctrlRequest(msgBytes);
}

int DiscreteControl::analogRead(AnalogInput_Num_t eana)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ANALOG_READ, (uint8_t)eana};
    ctrlRequest(msgBytes);
    int* ret = reinterpret_cast<int*>(&msgBytes[2]);
    return *ret;
}

void DiscreteControl::attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode)
{
    std::vector<uint8_t> msgBytes = {CONTROL_ATTACH_INTERRUPT, (uint8_t)din, (uint8_t)mode};
    _isrCallback[din] = callback;
    ctrlRequest(msgBytes);
    
    addEventCallback(
        EVENT_DIGITAL_INTERRUPT, 
        _id, 
        [this](uint8_t num) { _isrCallback[num](NULL); }
    );
}

void DiscreteControl::detachInterrupt(DigitalInputNum_t din)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DETACH_INTERRUPT, (uint8_t)din};
    ctrlRequest(msgBytes);
}

#endif
