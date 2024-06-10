/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "BusCtrlCmd_Relay.h"

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

void BusCtrlCmd_Relay::digitalWrite(Relay_Num_t num, uint8_t level)
{
    std::vector<uint8_t> msgBytes = {REQUEST_DIGITAL_WRITE, (uint8_t)num, (uint8_t)level};
    _control->request(msgBytes);
}

void BusCtrlCmd_Relay::toggleOutput(Relay_Num_t num)
{
    std::vector<uint8_t> msgBytes = {REQUEST_TOGGLE_OUTPUT, (uint8_t)num};
    _control->request(msgBytes);
}
#endif