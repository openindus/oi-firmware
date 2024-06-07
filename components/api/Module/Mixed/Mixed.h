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

#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define Mixed MixedStandalone
#elif defined(OI_MIXED) && defined(MODULE_SLAVE)
#define Mixed MixedSlave
#elif defined(MODULE_MASTER) 
#define Mixed MixedCmd
#endif

#if defined(OI_MIXED)

#include "MixedPinout.h"
#include "AnalogOutputs.h"
#include "AnalogInputsLV.h"
#include "DigitalOutputs.h"
#include "DigitalInputs.h"

class MixedStandalone : 
    public ModuleStandalone, 
    public AnalogOutputs, 
    public AnalogInputsLV, 
    public DigitalOutputs, 
    public DigitalInputs
{
public:

    static int init(void);
};

#include "CLI_AOut.h"
#include "CLI_AInLV.h"
#include "CLI_DIn.h"
#include "CLI_DOut.h"

class MixedCLI : 
    public CLI_AOut, 
    public CLI_AInLV, 
    public CLI_DIn, 
    public CLI_DOut
{
public:

    static int init(void);
};

#endif

#if defined(OI_MIXED) && defined(MODULE_SLAVE)

#include "BusCtrlSlave_AOut.h"
#include "BusCtrlSlave_AInLV.h"
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_DOut.h"

class MixedSlave : 
    public BusCtrlSlave, 
    public BusCtrlSlave_AOut, 
    public BusCtrlSlave_AInLV, 
    public BusCtrlSlave_DIn, 
    public BusCtrlSlave_DOut
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "BusCtrlCmd_AOut.h"
#include "BusCtrlCmd_AInLV.h"
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_DOut.h"

class MixedCmd : 
    public BusCtrlCmd, 
    public BusCtrlCmd_AOut, 
    public BusCtrlCmd_AInLV, 
    public BusCtrlCmd_DIn, 
    public BusCtrlCmd_DOut
{
public:

    MixedCmd(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_MIXED , sn),
        BusCtrlCmd_AOut(this),
        BusCtrlCmd_AInLV(this),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_DOut(this) {}
};
#endif


