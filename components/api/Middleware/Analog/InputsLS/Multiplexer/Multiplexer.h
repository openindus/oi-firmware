/**
 * @file Multiplexer.h
 * @brief Multiplexer
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

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
