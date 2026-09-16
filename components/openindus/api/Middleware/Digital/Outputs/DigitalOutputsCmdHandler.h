/**
 * @file DigitalOutputsCmdHandler.h
 * @brief Digital outputs commands handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Slave.h"
#include "DigitalOutputs.h"

#if defined(CONFIG_MODULE_SLAVE)

class DigitalOutputsCmdHandler
{
public:
    static int init(void) {
        Slave::addCallback(CALLBACK_DIGITAL_WRITE, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            digitalOutputs.digitalWrite((DoutNum_t)data[1], (bool)data[2]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_TOGGLE_OUTPUT, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            digitalOutputs.toggleOutput((DoutNum_t)data[1]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_OUTPUT_MODE, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            digitalOutputs.outputMode((DoutNum_t)data[1], (DoutMode_t)data[2]);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_SET_PWM_FREQUENCY, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            uint32_t *freq = reinterpret_cast<uint32_t *>(&data[2]);
            digitalOutputs.setPWMFrequency((DoutNum_t)data[1], *freq);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_SET_PWM_DUTY_CYCLE, [](std::vector<uint8_t> &data) {
            DoutNum_t num = (DoutNum_t)data[1];
            float *duty    = reinterpret_cast<float *>(&data[2]);
            DigitalOutputs digitalOutputs;
            digitalOutputs.setPWMDutyCycle(num, *duty);
            data.clear();
        });
    
        Slave::addCallback(CALLBACK_GET_OUTPUT_CURRENT, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            float current = digitalOutputs.getOutputCurrent((DoutNum_t)data[1]);
            uint8_t *ptr  = reinterpret_cast<uint8_t *>(&current);
            data.insert(data.end(), ptr, ptr + sizeof(float));
        });

        Slave::addCallback(CALLBACK_SET_OVERCURRENT_THRESHOLD, [](std::vector<uint8_t> &data) {
            float *threshold = reinterpret_cast<float *>(&data[1]);
            float *thresholdSum = reinterpret_cast<float *>(&data[5]);
            DigitalOutputs digitalOutputs;
            digitalOutputs.setOvercurrentThreshold(*threshold, *thresholdSum);
            data.clear();
        });

        Slave::addCallback(CALLBACK_ATTACH_OVERCURRENT_CALLBACK, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            digitalOutputs.attachOvercurrentCallback(_overcurrentCallback);
            data.clear();
        });

        Slave::addCallback(CALLBACK_DETACH_OVERCURRENT_CALLBACK, [](std::vector<uint8_t> &data) {
            DigitalOutputs digitalOutputs;
            digitalOutputs.detachOvercurrentCallback();
            data.clear();
        });

        Slave::addEventCallback(EVENT_CALLBACK_DIGITAL_WRITE, [](std::vector<uint8_t> &args) {
            DoutNum_t num = (DoutNum_t)args[0];
            bool level = (bool)args[1];
            DigitalOutputs digitalOutputs;
            digitalOutputs.digitalWrite(num, level);
        });

        Slave::addEventCallback(EVENT_CALLBACK_TOGGLE_OUTPUT, [](std::vector<uint8_t> &args) {
            DoutNum_t num = (DoutNum_t)args[0];
            DigitalOutputs digitalOutputs;
            digitalOutputs.toggleOutput(num);
        });

        return 0;
    }
private:
    static void (*_overcurrentCallback)(void*);

};

#endif