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
    EANA_A_P = 0,
    EANA_A_N,
    EANA_B_P,
    EANA_B_N,
    EANA_C_P,
    EANA_C_N,
    EANA_D_P,
    EANA_D_N,
    EANA_E_P,
    EANA_E_N,
    EANA_MAX
} EAna_Num_t;

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

};

class Digipot
{
public:

    Digipot() {}

};

class Excitation
{
public:

    Excitation() {}

private:

    Multiplexer* _highSideMux;
    Multiplexer* _lowSideMux;
    Digipot* _digipot;

};

class AnalogInputsLS
{
protected:

    static int init(ADC_Device* adc, Excitation* excit);

private:

    static ADC_Device* _adc;
    static Excitation _excit;

};