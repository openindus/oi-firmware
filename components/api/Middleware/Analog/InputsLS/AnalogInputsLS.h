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
    AINLS_A_P = 0,
    AINLS_A_N,
    AINLS_B_P,
    AINLS_B_N,
    AINLS_C_P,
    AINLS_C_N,
    AINLS_D_P,
    AINLS_D_N,
    AINLS_E_P,
    AINLS_E_N,
    AINLS_MAX
} AInLS_Num_t;

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

class Multiplexer
{
public:

    Multiplexer(const std::array<gpio_num_t, 3>& inputPins, 
                const std::array<gpio_num_t, 3>& outputPins)
        : _inputPins(inputPins), _outputPins(outputPins) {
    }

    int init(void);

    int route(int input, int output);

private:

    std::array<gpio_num_t, 3> _inputPins; // A0, A1, A2
    std::array<gpio_num_t, 3> _outputPins; // B0, B1, B2

};

class Digipot
{
public:

    Digipot() {}

};

class AnalogInputsLS
{
public: 

    static int test2WireRTD(void);

protected:

    static int init(void);

    /* ADC */
    static ADS114S0X* _adc;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    static Digipot* _digipot;

};