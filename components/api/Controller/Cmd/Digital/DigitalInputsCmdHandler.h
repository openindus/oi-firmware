/**
 * @file DigitalInputsCmdHandler.h
 * @brief Digital inputs commands handler
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "DigitalInputs.h"
#include "Slave.h"

class DigitalInputsCmdHandler
{
public:
    static int init(void)
    {
        Slave::addRequestCallback(REQUEST_DIGITAL_READ, [](std::vector<uint8_t> &data) {
            int level = DigitalInputs::digitalRead((DIn_Num_t)data[1]);
            data.push_back(static_cast<uint8_t>(level));
        });

        Slave::addRequestCallback(REQUEST_ATTACH_INTERRUPT, [](std::vector<uint8_t> &data) {
            DigitalInputs::attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]],
                                           (InterruptMode_t)data[2]);
            data.clear();
        });

        Slave::addRequestCallback(REQUEST_DETACH_INTERRUPT, [](std::vector<uint8_t> &data) {
            DigitalInputs::detachInterrupt((DIn_Num_t)data[1]);
            data.clear();
        });

        Slave::addResetFunction([]() {
            for (int i = 0; i < DIN_MAX; ++i) {
                DigitalInputs::detachInterrupt((DIn_Num_t)i);
            }
        });

        return 0;
    }

private:
    static IsrCallback_t _isrCallback[DIN_MAX];
};

#endif