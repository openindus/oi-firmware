/**
 * @file Discrete.h
 * @brief Discrete
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_STANDALONE)
#define Discrete DiscreteStandalone
#elif (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)
#define Discrete DiscreteSlave
#elif defined(MODULE_MASTER) 
#define Discrete DiscreteMaster
#endif


#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

#include "DiscretePinout.h"
#include "DigitalInputs.h"
#include "DigitalOutputs.h"
#include "AnalogInputsHV.h"

class DiscreteStandalone : 
    public ModuleStandalone, 
    public DigitalInputs, 
    public DigitalOutputs, 
    public AnalogInputsHV
{
public:

    static int init(void);
};

#include "CLI_DIn.h"
#include "CLI_DOut.h"
#include "CLI_AInHV.h"

class DiscreteCLI : 
    public CLI_AInHV, 
    public CLI_DIn, 
    public CLI_DOut
{
public:

    static int init(void);
};

#endif

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)

#include "Slave.h"
#include "ResponseDIn.h"
#include "ResponseDOut.h"
#include "ResponseAInHV.h"

class DiscreteSlave : 
    public Slave, 
    public ResponseDIn, 
    public ResponseDOut, 
    public ResponseAInHV
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER) 

#include "Command.h"
#include "CommandDIn.h"
#include "CommandDOut.h"
#include "CommandAInHV.h"

class DiscreteMaster : 
    public Command, 
    public CommandDIn, 
    public CommandDOut, 
    public CommandAInHV
{
public:

    DiscreteMaster(uint32_t sn = 0) : 
        Command(TYPE_OI_DISCRETE, sn),
        CommandDIn(this),
        CommandDOut(this),
        CommandAInHV(this) {}
};
#endif