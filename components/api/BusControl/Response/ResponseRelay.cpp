/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ResponseRelay.h"

#if defined(MODULE_SLAVE)

int ResponseRelay::init() {
    
    Slave::addCtrlCallback(REQUEST_DIGITAL_WRITE, [](std::vector<uint8_t>& data) {
        Relays::digitalWrite((Relay_Num_t)data[1], data[2]);
        data.clear();
    });

    Slave::addCtrlCallback(REQUEST_TOGGLE_OUTPUT, [](std::vector<uint8_t>& data) {
        Relays::toggleOutput((Relay_Num_t)data[1]);
        data.clear();
    });

    return 0;
}

#endif