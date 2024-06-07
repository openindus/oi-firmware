/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Relays.h"

class CLI_Relay : public Relays
{
public:

    static inline int init(void) {
        int err = 0;
        err |= _registerDigitalWrite();
        return err;
    };

private:
    static int _registerDigitalWrite();
};