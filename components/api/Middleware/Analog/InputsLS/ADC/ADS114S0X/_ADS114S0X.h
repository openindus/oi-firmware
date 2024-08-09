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
#define ADS114S0X_INPUT_MAX 12

enum ADS114S0X_Gain_e : int {
    GAIN_1      = 1,
    Gain_2      = 2,
    GAIN_4      = 4,
    GAIN_8      = 8,
    GAIN_16     = 16,
    GAIN_32     = 32,
    GAIN_64     = 64,
    GAIN_128    = 128
};

enum ADS114S0X_Reference_e : int {
    REF_EXTERNAL_IDAC1  = 1,
    REF_INTERNAL_2V5    = 2,
};

typedef int ADC_Input_t;

class ADS114S0X
{
public:

    ADS114S0X(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config), _convTimeMs(1000) {}

    int init(void);

    inline void setConvTimeMs(uint32_t convTimeMs) {
        _convTimeMs = convTimeMs;
    };

    int config(ADS114S0X_Gain_e gain, ADS114S0X_Reference_e reference, bool useExcitation);
    int read(std::vector<uint16_t>* adcCode, 
        ADC_Input_t inputP, ADC_Input_t inputN, bool useVbias=false);

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;

    uint32_t _convTimeMs;

    static QueueHandle_t _queue;
    static void IRAM_ATTR _isr(void* arg);
    static void _task(void* arg);

    static std::vector<uint16_t> _data;

};