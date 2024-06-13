/**
 * @file Mixed.h
 * @brief Mixed
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"
#include "MixedPinout.h"
#include "AnalogOutputs.h"
#include "AnalogInputsLV.h"
#include "DigitalOutputs.h"
#include "DigitalInputs.h"
#include "CLI_AOut.h"
#include "CLI_AInLV.h"
#include "CLI_DIn.h"
#include "CLI_DOut.h"
#include "ResponseAOut.h"
#include "ResponseAInLV.h"
#include "ResponseDIn.h"
#include "ResponseDOut.h"
#include "CommandAOut.h"
#include "CommandAInLV.h"
#include "CommandDIn.h"
#include "CommandDOut.h"

#if defined(OI_MIXED)

class Mixed : public Module, 
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
    public CommandAOut, 
    public CommandAInLV, 
    public CommandDIn, 
    public CommandDOut
{
public:

    Mixed(uint32_t sn = 0) : 
        Controller(TYPE_OI_MIXED , sn),
        CommandAOut(this),
        CommandAInLV(this),
        CommandDIn(this),
        CommandDOut(this) {}

};

#endif