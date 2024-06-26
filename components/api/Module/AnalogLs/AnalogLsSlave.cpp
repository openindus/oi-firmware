/**
 * @file AnalogLsSlave.cpp
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogLsSlave.h"

#if defined(OI_ANALOG_LS)

AnalogLsStandalone* AnalogLsSlave::_analogLs = new AnalogLsStandalone();

int AnalogLsSlave::init(void)
{
    ModuleSlave::init();

    return 0;
}

#endif