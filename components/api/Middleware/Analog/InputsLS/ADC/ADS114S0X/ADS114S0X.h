/**
 * @file ADS114S0X.h
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

#define ADS114S0X_R_REF 1000
#define ADS114S0X_GAIN 4
#define ADS114S0X_RES 16

typedef enum ads114s0x_adc_input_e ADC_Input_t;

class ADS114S0X
{
public:

    ADS114S0X(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config) {}

    int init(void);
    int config(void);

    int read(std::vector<uint16_t>* adcCode, ADC_Input_t inputP, ADC_Input_t inputN, uint32_t timeout_ms);

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;

    static QueueHandle_t _queue;
    static void IRAM_ATTR _isr(void* arg);
    static void _task(void* arg);

    static std::vector<uint16_t> _data;

};