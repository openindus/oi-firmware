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
#define Mixed MixedMaster
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

#include "ResponseAOut.h"
#include "ResponseAInLV.h"
#include "ResponseDIn.h"
#include "ResponseDOut.h"

class MixedSlave : 
    public Slave, 
    public ResponseAOut, 
    public ResponseAInLV, 
    public ResponseDIn, 
    public ResponseDOut
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

#include "CommandAOut.h"
#include "CommandAInLV.h"
#include "CommandDIn.h"
#include "CommandDOut.h"

class MixedMaster : 
    public Command, 
    public CommandAOut, 
    public CommandAInLV, 
    public CommandDIn, 
    public CommandDOut
{
public:

    MixedMaster(uint32_t sn = 0) : 
        Command(TYPE_OI_MIXED , sn),
        CommandAOut(this),
        CommandAInLV(this),
        CommandDIn(this),
        CommandDOut(this) {}
};
#endif


