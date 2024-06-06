/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlCmd_DOut.cpp
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "ControlCmd_DOut.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void ControlCmd_DOut::digitalWrite(DOut_Num_t num, uint8_t level)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _control->request(msgBytes);
}

void ControlCmd_DOut::toggleOutput(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_TOGGLE_OUTPUT, (uint8_t)num};
    _control->request(msgBytes);
}

void ControlCmd_DOut::outputMode(DOut_Num_t num, DOut_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    _control->request(msgBytes);
}

void ControlCmd_DOut::setPWMFrequency(DOut_Num_t num, uint32_t freq)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_FREQUENCY, (uint8_t)num};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&freq);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}
    
void ControlCmd_DOut::setPWMDutyCycle(DOut_Num_t num, uint32_t duty)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_DUTY_CYCLE, (uint8_t)num};
    uint8_t* ptr = reinterpret_cast<uint8_t*>(&duty);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _control->request(msgBytes);
}

float ControlCmd_DOut::getOutputCurrent(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_GET_OUTPUT_CURRENT, (uint8_t)num};
    _control->request(msgBytes);
    float* current = reinterpret_cast<float*>(&msgBytes[2]);
    return *current;
}


#endif