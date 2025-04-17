/**
 * @file DigitalOutputsCmd.cpp
 * @brief Digital outputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCmd.h"

#if defined(MODULE_MASTER)

void DigitalOutputsCmd::digitalWrite(DOut_Num_t num, bool level)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::toggleOutput(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_TOGGLE_OUTPUT, (uint8_t)num};
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::outputMode(DOut_Num_t num, DOut_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {REQUEST_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::setPWMFrequency(DOut_Num_t num, uint32_t freq)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_FREQUENCY, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&freq);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::setPWMDutyCycle(DOut_Num_t num, float duty)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_PWM_DUTY_CYCLE, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&duty);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _controller->performRequest(msgBytes);
}

float DigitalOutputsCmd::getOutputCurrent(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_GET_OUTPUT_CURRENT, (uint8_t)num};
    _controller->performRequest(msgBytes);
    float *current = reinterpret_cast<float *>(&msgBytes[2]);
    return *current;
}

void DigitalOutputsCmd::setOvercurrentThreshold(float threshold, float thresholdSum)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_OVERCURRENT_THRESHOLD};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&threshold);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ptr = reinterpret_cast<uint8_t *>(&thresholdSum);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::attachOvercurrentCallback(void (*callback)(void))
{
    std::vector<uint8_t> msgBytes = {REQUEST_ATTACH_OVERCURRENT_CALLBACK};
    _overcurrentCallback          = callback;
    Master::addEventCallback(EVENT_OVERCURRENT, _controller->getId(),
                             [this](uint8_t *data) { _overcurrentCallback(); });
    _controller->performRequest(msgBytes);
}

void DigitalOutputsCmd::detachOvercurrentCallback()
{
    std::vector<uint8_t> msgBytes = {REQUEST_DETACH_OVERCURRENT_CALLBACK};
    _controller->performRequest(msgBytes);
}

#endif