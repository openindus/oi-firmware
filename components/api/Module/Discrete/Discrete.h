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
#include "ControlSlave_AInHV.h"
#include "ControlSlave_DIn.h"
#include "ControlSlave_DOut.h"
#elif defined(MODULE_MASTER) 
#define Discrete DiscreteControl
#include "ControlCmd_AInHV.h"
#include "ControlCmd_DIn.h"
#include "ControlCmd_DOut.h"
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
class DiscreteSlave : public ControlSlave, public DiscreteStandalone, public ControlSlave_DIn, public ControlSlave_DOut, public ControlSlave_AInHV
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER) 
class DiscreteControl : public ControlCmd, public ControlCmd_DIn, public ControlCmd_DOut, public ControlCmd_AInHV
{
public:

    DiscreteControl(uint32_t sn = 0) : 
        ControlCmd(TYPE_OI_DISCRETE, sn),
        ControlCmd_DIn(this),
        ControlCmd_DOut(this),
        ControlCmd_AInHV(this) {}
};
#endif