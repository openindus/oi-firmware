/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputsControl.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "DigitalOutputsControl.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void DigitalOutputsControl::digitalWrite(DigitalOutputNum_t num, uint8_t level)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _control->ctrlRequest(msgBytes);
}

void DigitalOutputsControl::digitalToggle(DigitalOutputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_TOGGLE, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
}

void DigitalOutputsControl::digitalModePWM(DigitalOutputNum_t num, uint32_t freq)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_MODE_PWM, (uint8_t)num};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&freq);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}
    
void DigitalOutputsControl::digitalSetPWM(DigitalOutputNum_t num, uint32_t duty)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_SET_PWM, (uint8_t)num};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&duty);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->ctrlRequest(msgBytes);
}

float DigitalOutputsControl::digitalGetCurrent(DigitalOutputNum_t num)
{
    std::vector<uint8_t> msgBytes = {CONTROL_DIGITAL_GET_CURRENT, (uint8_t)num};
    _control->ctrlRequest(msgBytes);
    float* current = reinterpret_cast<float*>(&msgBytes[2]);
    return *current;
}


#endif