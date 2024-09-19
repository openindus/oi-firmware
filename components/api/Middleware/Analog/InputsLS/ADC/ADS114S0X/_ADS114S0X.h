/**
 * @file ADS114S0X.h
 * @brief ADS114S0X
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

#define ADS114S0X_RESOLUTION        16
#define ADS114S0X_MAX_ADC_CODE      65535
#define ADS114S0X_INPUT_MAX         12

typedef int ADC_Input_t;

class ADS114S0X
{
public:

    ADS114S0X(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config) {}

    int init(void);

    int setDataRate(ads114s0x_data_rate_e dataRate);

    int setInternalMux(ads114s0x_adc_input_e inputP, ads114s0x_adc_input_e inputN);

    int setPGAGain(ads114s0x_pga_gain_e gain);

    int setReference(ads114s0x_ref_selection_e reference);

    int setExcitation(ads114s0x_idac_magnitude_e current);

    int setBias(ads114s0x_adc_input_e input);

    int read();

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;

    static QueueHandle_t _queue;
    static void IRAM_ATTR _isr(void* arg);
};