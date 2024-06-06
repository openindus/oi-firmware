/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Discrete.h
 * @brief Functions for Discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
#include "DiscretePinout.h"
#include "CLI_DIn.h"
#include "CLI_DOut.h"
#include "CLI_AInHV.h"
#endif

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_STANDALONE)
#define Discrete DiscreteStandalone
#elif (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)
#define Discrete DiscreteSlave
#include "BusCtrlSlave_AInHV.h"
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_DOut.h"
#elif defined(MODULE_MASTER) 
#define Discrete DiscreteBusCtrl
#include "BusCtrlCmd_AInHV.h"
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_DOut.h"
#endif


#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
class DiscreteCLI : public CLI_AInHV, public CLI_DIn, public CLI_DOut
{
public:

    static int init(void);
};

class DiscreteStandalone : public ModuleStandalone, public DigitalInputs, public DigitalOutputs, public AnalogInputsHV
{
public:

    static int init(void);
};
#endif

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)
class DiscreteSlave : public BusCtrlSlave, public DiscreteStandalone, public BusCtrlSlave_DIn, public BusCtrlSlave_DOut, public BusCtrlSlave_AInHV
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER) 
class DiscreteBusCtrl : public BusCtrlCmd, public BusCtrlCmd_DIn, public BusCtrlCmd_DOut, public BusCtrlCmd_AInHV
{
public:

    DiscreteBusCtrl(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_DISCRETE, sn),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_DOut(this),
        BusCtrlCmd_AInHV(this) {}
};
#endif