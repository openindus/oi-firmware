/**
 * @file ADS114S0X.h
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class ADS114S0X
{
public:

    ADS114S0X(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config) {}

    int init(void);

    int config(void);
    int startConversion(void);

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;

    static QueueHandle_t _queue;
    static void IRAM_ATTR _isr(void* arg);
    static void _task(void* arg);

};