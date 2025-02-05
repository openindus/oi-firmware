/**
 * @file Bus.h
 * @brief Bus
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "BusIO.h"
#include "BusCAN.h"
#include "BusRS.h"

class Bus : 
    public BusRS,
    public BusCAN,
    public BusIO
{
private:

    static int _registerCLI(void);

public:

    static inline int init(void) {
        return _registerCLI();
    }

};