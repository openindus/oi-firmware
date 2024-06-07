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
#define Discrete DiscreteCmd
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

#include "BusCtrlSlave.h"
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_DOut.h"
#include "BusCtrlSlave_AInHV.h"

class DiscreteSlave : 
    public BusCtrlSlave, 
    public BusCtrlSlave_DIn, 
    public BusCtrlSlave_DOut, 
    public BusCtrlSlave_AInHV
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER) 

#include "BusCtrlCmd.h"
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_DOut.h"
#include "BusCtrlCmd_AInHV.h"

class DiscreteCmd : 
    public BusCtrlCmd, 
    public BusCtrlCmd_DIn, 
    public BusCtrlCmd_DOut, 
    public BusCtrlCmd_AInHV
{
public:

    DiscreteCmd(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_DISCRETE, sn),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_DOut(this),
        BusCtrlCmd_AInHV(this) {}
};
#endif