/**
 * @file AnalogLS.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"
#include "AnalogLSConfig.h"
#include "AnalogInputsLS.h"
#include "AnalogInputsLSCmdHandler.h"
#include "AnalogInputsLSCmd.h"

#if (OI_ANALOG_LS)

class AnalogLS:
    public Module,
    public AnalogInputsLS
{
public:

    static int init(void);

};

#elif defined(MODULE_MASTER) 

class AnalogLS :
    public Controller, 
    public AnalogInputsLSCmd
{
public:

    AnalogLS(uint32_t sn = 0) : 
        Controller(TYPE_OI_ANALOG_LS , sn),
        AnalogInputsLSCmd(this) {}
};

#endif