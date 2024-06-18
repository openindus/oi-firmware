/**
 * @file AnalogInputsLS.h
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

typedef enum {
    AIN_A_P = 0,
    AIN_A_N,
    AIN_B_P,
    AIN_B_N,
    AIN_C_P,
    AIN_C_N,
    AIN_D_P,
    AIN_D_N,
    AIN_E_P,
    AIN_E_N,
    AIN_MAX
} AIn_Num_t;

class ADC_Device
{
public:

    ADC_Device(ads114s0x_device_t* device, ads114s0x_config_t config) :
        _device(device), _config(config) {}

    int init(void);
    int test(void);

private:

    ads114s0x_device_t* _device;
    ads114s0x_config_t _config;
};

class Multiplexer
{
public:

    Multiplexer() {}

    int route(int input, int output);

private:

};

class Digipot
{
public:

    Digipot() {}

};

class AnalogInputsLS
{
protected:

    static int init(void);

    /* ADC */
    static ADC_Device* _adcDevice;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    static Digipot* _digipot;

};