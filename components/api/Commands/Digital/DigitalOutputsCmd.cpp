/**
 * @file DigitalOutputsCmd.cpp
 * @brief Digital outputs commands
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCmd.h"
#include "Commands.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void DigitalOutputsCmd::digitalWrite(DOut_Num_t num, bool level)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _controllerInst->sendRequest(msgBytes);
}

void DigitalOutputsCmd::toggleOutput(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_TOGGLE_OUTPUT, (uint8_t)num};
    _controllerInst->sendRequest(msgBytes);
}

void DigitalOutputsCmd::outputMode(DOut_Num_t num, DOut_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    _controllerInst->sendRequest(msgBytes);
}

void DigitalOutputsCmd::setPWMFrequency(DOut_Num_t num, uint32_t freq)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_FREQUENCY, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&freq);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _controllerInst->sendRequest(msgBytes);
}

void DigitalOutputsCmd::setPWMDutyCycle(DOut_Num_t num, uint32_t duty)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_DUTY_CYCLE, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&duty);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _controllerInst->sendRequest(msgBytes);
}

float DigitalOutputsCmd::getOutputCurrent(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_GET_OUTPUT_CURRENT, (uint8_t)num};
    _controllerInst->sendRequest(msgBytes);
    float *current = reinterpret_cast<float *>(&msgBytes[2]);
    return *current;
}

#endif