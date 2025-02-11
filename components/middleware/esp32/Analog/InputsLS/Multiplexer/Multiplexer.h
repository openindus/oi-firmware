/**
 * @file Multiplexer.h
 * @brief Multiplexer
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>
#include <array>
#include "driver/gpio.h"

#define MUX_IO_MAX 7

enum Multiplexer_Input_e : int8_t {
    INPUT_OPEN_HS       = 0,
    INPUT_IDAC1         = 1,
    INPUT_IDAC2         = 2,
    INPUT_SENSOR_5V     = 3,
    INPUT_SENSOR_3V3    = 4,
    INPUT_VISUP_DIV     = 5
};

enum Multiplexer_Output_e : int8_t {
    OUTPUT_OPEN_LS      = 0,
    OUTPUT_W_DIGIPOT    = 1,
    OUTPUT_RBIAS_GAUGE  = 2,
    OUTPUT_RBIAS_RTD    = 3,
    OUTPUT_GND          = 4
};

typedef int8_t Mux_IO_t;

class Multiplexer
{
public:

    Multiplexer(const std::array<gpio_num_t, 3>& inputPins, 
                const std::array<gpio_num_t, 3>& outputPins)
        : _inputPins(inputPins), _outputPins(outputPins) {
    }

    int init(void);

    int route(Mux_IO_t input, Mux_IO_t output);

private:

    std::array<gpio_num_t, 3> _inputPins; // A0, A1, A2
    std::array<gpio_num_t, 3> _outputPins; // B0, B1, B2

};
