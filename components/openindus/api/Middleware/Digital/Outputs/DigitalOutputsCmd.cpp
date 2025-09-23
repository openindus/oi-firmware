/**
 * @file DigitalOutputsCmd.cpp
 * @brief Digital Outputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "DigitalOutputsCmd.h"

#if defined(CONFIG_MODULE_MASTER)

DOut_Num_t doutNumTable[DOUT_MAX] = {DOUT_1, DOUT_2, DOUT_3, DOUT_4, 
#if !defined(CONFIG_OI_CORE) and !defined(CONFIG_OI_CORE_LITE)
                                     DOUT_5, DOUT_6, DOUT_7, DOUT_8
#endif
                                    };

void DigitalOutputsCmd::digitalWrite(DOut_Num_t num, bool level)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::toggleOutput(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_TOGGLE_OUTPUT, (uint8_t)num};
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::outputMode(DOut_Num_t num, DOut_Mode_t mode)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_OUTPUT_MODE, (uint8_t)num, (uint8_t)mode};
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::setPWMFrequency(DOut_Num_t num, uint32_t freq)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_SET_PWM_FREQUENCY, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&freq);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::setPWMDutyCycle(DOut_Num_t num, float duty)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_SET_PWM_DUTY_CYCLE, (uint8_t)num};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&duty);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

float DigitalOutputsCmd::getOutputCurrent(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_GET_OUTPUT_CURRENT, (uint8_t)num};
    _module->runCallback(msgBytes);
    float *current = reinterpret_cast<float *>(&msgBytes[2]);
    return *current;
}

int DigitalOutputsCmd::outputIsOvercurrent(DOut_Num_t num)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_OUTPUT_IS_OVERCURRENT, (uint8_t)num};
    _module->runCallback(msgBytes);
    return static_cast<int>(msgBytes[2]);
}

void DigitalOutputsCmd::setOvercurrentThreshold(float threshold, float thresholdSum)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_SET_OVERCURRENT_THRESHOLD};
    uint8_t *ptr                  = reinterpret_cast<uint8_t *>(&threshold);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    ptr = reinterpret_cast<uint8_t *>(&thresholdSum);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(float));
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::attachOvercurrentCallback(void (*callback)(void*), void *arg)
{
    std::vector<uint8_t> msgBytes = {CALLBACK_ATTACH_OVERCURRENT_CALLBACK};
    _overcurrentCallback          = callback;
    Master::addEventCallback(EVENT_OVERCURRENT, _module->getId(),
                             [this](uint8_t *data) { 
                                if (_overcurrentCallback != NULL) 
                                    _overcurrentCallback(&doutNumTable[data[1]]); 
                             });
    _module->runCallback(msgBytes);
}

void DigitalOutputsCmd::detachOvercurrentCallback()
{
    std::vector<uint8_t> msgBytes = {CALLBACK_DETACH_OVERCURRENT_CALLBACK};
    _module->runCallback(msgBytes);
}

#endif