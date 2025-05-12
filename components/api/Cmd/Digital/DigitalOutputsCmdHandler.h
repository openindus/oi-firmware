/**
 * @file DigitalOutputsCmdHandler.h
 * @brief Digital outputs commands handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "Slave.h"
#include "DigitalOutputs.h"

class DigitalOutputsCmdHandler
{
public:

    static int init(void) {
        Slave::addCallback(CALLBACK_DIGITAL_WRITE, [](std::vector<uint8_t> &data) {
            DigitalOutputs::digitalWrite((DOut_Num_t)data[1], (bool)data[2]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_TOGGLE_OUTPUT, [](std::vector<uint8_t> &data) {
            DigitalOutputs::toggleOutput((DOut_Num_t)data[1]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_OUTPUT_MODE, [](std::vector<uint8_t> &data) {
            DigitalOutputs::outputMode((DOut_Num_t)data[1], (DOut_Mode_t)data[2]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_SET_PWM_FREQUENCY, [](std::vector<uint8_t> &data) {
            uint32_t *freq = reinterpret_cast<uint32_t *>(&data[2]);
            DigitalOutputs::setPWMFrequency((DOut_Num_t)data[1], *freq);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_SET_PWM_DUTY_CYCLE, [](std::vector<uint8_t> &data) {
            DOut_Num_t num = (DOut_Num_t)data[1];
            float *duty    = reinterpret_cast<float *>(&data[2]);
            DigitalOutputs::setPWMDutyCycle(num, *duty);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_GET_OUTPUT_CURRENT, [](std::vector<uint8_t> &data) {
            float current = DigitalOutputs::getOutputCurrent((DOut_Num_t)data[1]);
            uint8_t *ptr  = reinterpret_cast<uint8_t *>(&current);
            data.insert(data.end(), ptr, ptr + sizeof(float));
        });

        Slave::addCallback(CALLBACK_SET_OVERCURRENT_THRESHOLD, [](std::vector<uint8_t> &data) {
            float *threshold = reinterpret_cast<float *>(&data[1]);
            float *thresholdSum = reinterpret_cast<float *>(&data[5]);
            DigitalOutputs::setOvercurrentThreshold(*threshold, *thresholdSum);
            data.clear();
        });

        Slave::addCallback(CALLBACK_ATTACH_OVERCURRENT_CALLBACK, [](std::vector<uint8_t> &data) {
            DigitalOutputs::attachOvercurrentCallback(_overcurrentCallback);
            data.clear();
        });

        Slave::addCallback(CALLBACK_DETACH_OVERCURRENT_CALLBACK, [](std::vector<uint8_t> &data) {
            DigitalOutputs::detachOvercurrentCallback();
            data.clear();
        });

        Slave::addEventCallback(EVENT_CALLBACK_DIGITAL_WRITE, [](std::vector<uint8_t> &args) {
            DOut_Num_t num = (DOut_Num_t)args[0];
            bool level = (bool)args[1];
            DigitalOutputs::digitalWrite(num, level);
        });

        return 0;
    }
private:

    static void (*_overcurrentCallback)(void);
};

#endif