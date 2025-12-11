/**
 * @file Discrete.h
 * @brief Discrete Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"
#include "DiscretePinout.h"
#include "DigitalInputs.h"
#include "DigitalOutputs.h"
#include "AnalogInputsHV.h"
#include "Slave.h"
#include "DigitalInputsCmdHandler.h"
#include "DigitalOutputsCmdHandler.h"
#include "AnalogInputsHVCmdHandler.h"
#include "ModuleControl.h"
#include "DigitalInputsCmd.h"
#include "DigitalOutputsCmd.h"
#include "AnalogInputsHVCmd.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"

#if (defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DISCRETE_VE))

class Discrete : public Module, 
    public DigitalInputs, 
    public DigitalOutputs, 
    public AnalogInputsHV
{
public:

    static int init(void);

};

/* DiscreteVE is identical to Discrete */
#define DiscreteVE Discrete

#elif defined(CONFIG_MODULE_MASTER) 

class Discrete : 
    public ModuleControl, 
    public DigitalInputsCmd, 
    public DigitalOutputsCmd, 
    public AnalogInputsHVCmd
{
public:

    Discrete(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_DISCRETE, sn),
        DigitalInputsCmd(this),
        DigitalOutputsCmd(this),
        AnalogInputsHVCmd(this) {}
};

class DiscreteVE :
    public ModuleControl, 
    public DigitalInputsCmd, 
    public DigitalOutputsCmd, 
    public AnalogInputsHVCmd
{
public:

    DiscreteVE(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_DISCRETE_VE, sn),
        DigitalInputsCmd(this),
        DigitalOutputsCmd(this),
        AnalogInputsHVCmd(this) {}
};

#endif