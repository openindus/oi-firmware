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
        Slave::addCallback(CALLBACK_DIGITAL_READ, [](std::vector<uint8_t> &data) {
            DigitalInputs digitalInputs;
            int level = digitalInputs.digitalRead((DIn_Num_t)data[1]);
            data.push_back(static_cast<uint8_t>(level));
        });

        Slave::addCallback(CALLBACK_ATTACH_INTERRUPT, [](std::vector<uint8_t> &data) {
            DigitalInputs digitalInputs;
            digitalInputs.attachInterrupt((DIn_Num_t)data[1], _isrCallback[data[1]],
                                           (InterruptMode_t)data[2]);
            data.clear();
        });

        Slave::addCallback(CALLBACK_DETACH_INTERRUPT, [](std::vector<uint8_t> &data) {
            DigitalInputs digitalInputs;
            digitalInputs.detachInterrupt((DIn_Num_t)data[1]);
            data.clear();
        });

        Slave::addResetCallback([]() {
            for (int i = 0; i < DIN_MAX; ++i) {
                DigitalInputs digitalInputs;
                digitalInputs.detachInterrupt((DIn_Num_t)i);
            }
        });

        return 0;
    }

private:
    static IsrCallback_t _isrCallback[DIN_MAX];
};

#endif