/**
 * @file Multiplexer.h
 * @brief Multiplexer
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

enum Multiplexer_Input_e {
    INPUT_OPEN_HS       = (int) 0,
    INPUT_IDAC1         = (int) 1,
    INPUT_IDAC2         = (int) 2,
    INPUT_SENSOR_5V     = (int) 3,
    INPUT_SENSOR_3V3    = (int) 4,
    INPUT_VISUP_DIV     = (int) 5
};

enum Multiplexer_Output_e {
    OUTPUT_OPEN_LS      = (int) 0,
    OUTPUT_W_DIGIPOT    = (int) 1,
    OUTPUT_RBIAS_GAUGE  = (int) 2,
    OUTPUT_RBIAS_RTD    = (int) 3,
    OUTPUT_GND          = (int) 4
};

typedef int Multiplexer_IO_t;

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
