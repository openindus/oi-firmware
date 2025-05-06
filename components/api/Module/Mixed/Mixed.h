/**
 * @file Mixed.h
 * @brief Mixed
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"
#include "MixedPinout.h"
#include "AnalogOutputs.h"
#include "AnalogInputsLV.h"
#include "DigitalOutputs.h"
#include "DigitalDefs.h"
#include "DigitalInputs.h"
#include "AnalogOutputsCmdHandler.h"
#include "AnalogInputsLVCmdHandler.h"
#include "DigitalInputsCmdHandler.h"
#include "DigitalOutputsCmdHandler.h"
#include "AnalogOutputsCmd.h"
#include "AnalogInputsLVCmd.h"
#include "DigitalInputsCmd.h"
#include "DigitalOutputsCmd.h"

#if defined(OI_MIXED)

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

#elif defined(MODULE_MASTER) 

class Mixed : 
    public Controller, 
    public AnalogOutputsCmd, 
    public AnalogInputsLVCmd, 
    public DigitalInputsCmd, 
    public DigitalOutputsCmd
{
public:

    Mixed(uint32_t sn = 0) : 
        Controller(TYPE_OI_MIXED , sn),
        AnalogOutputsCmd(this),
        AnalogInputsLVCmd(this),
        DigitalInputsCmd(this),
        DigitalOutputsCmd(this) {}

};

#endif