/**
 * @file Mixed.h
 * @brief Mixed Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"
#include "AnalogOutputs.h"
#include "AnalogInputsLV.h"
#include "DigitalOutputs.h"
#include "DigitalInputs.h"
#include "AnalogOutputsCmdHandler.h"
#include "AnalogInputsLVCmdHandler.h"
#include "DigitalInputsCmdHandler.h"
#include "DigitalOutputsCmdHandler.h"
#include "AnalogOutputsCmd.h"
#include "AnalogInputsLVCmd.h"
#include "DigitalInputsCmd.h"
#include "DigitalOutputsCmd.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"

#if defined(CONFIG_OI_MIXED)

class Mixed : 
    public Module, 
    public AnalogOutputs, 
    public AnalogInputsLV, 
    public DigitalOutputs, 
    public DigitalInputs
{
public:

    static int init(void);

};

#elif defined(CONFIG_MODULE_MASTER) 

class Mixed : 
    public ModuleControl, 
    public AnalogOutputsCmd, 
    public AnalogInputsLVCmd, 
    public DigitalInputsCmd, 
    public DigitalOutputsCmd
{
public:

    Mixed(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_MIXED , sn),
        AnalogOutputsCmd(this),
        AnalogInputsLVCmd(this),
        DigitalInputsCmd(this),
        DigitalOutputsCmd(this) {}

};

#endif