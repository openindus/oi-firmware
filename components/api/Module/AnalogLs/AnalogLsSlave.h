/**
 * @file AnalogLsSlave.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ModuleSlave.h"
#include "AnalogLsStandalone.h"

#if defined(OI_ANALOG_LS) && defined(MODULE_SLAVE)

class AnalogLsSlave : public ModuleSlave
{
public:

    static int init(void);
    
private:

    static AnalogLsStandalone* _analogLs;

};

#endif