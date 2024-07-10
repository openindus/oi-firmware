/**
 * @file ADS114S0X.h
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

#define ADS114S0X_RESOLUTION 16

enum ADS114S0X_Gain_e {
    GAIN_1      = (int) 1,
    Gain_2      = (int) 2,
    GAIN_4      = (int) 4,
    GAIN_8      = (int) 8,
    GAIN_16     = (int) 16,
    GAIN_32     = (int) 32,
    GAIN_64     = (int) 64,
    GAIN_128    = (int) 128
};

enum ADS114S0X_Reference_e {
    REF_EXTERNAL_IDAC1  = (int) 1,
    REF_INTERNAL_2V5    = (int) 2,
};

typedef int ADC_Input_t;

class ADS114S0X
{
public:

    ADS114S0X(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config) {}

    int init(void);

    int config(int gain, int reference, bool useExcitation);
    int read(std::vector<uint16_t>* adcCode, ADC_Input_t inputP, ADC_Input_t inputN, 
        uint32_t timeMs=1000, bool useVbias=false);

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;

    static QueueHandle_t _queue;
    static void IRAM_ATTR _isr(void* arg);
    static void _task(void* arg);

    static std::vector<uint16_t> _data;

};