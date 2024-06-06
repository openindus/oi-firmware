/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Mixed.h
 * @brief Functions for Mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if defined(OI_MIXED)
#include "MixedPinout.h"
#include "CLI_AOut.h"
#include "CLI_AInLV.h"
#include "CLI_DIn.h"
#include "CLI_DOut.h"
#endif

#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define Mixed MixedStandalone
#elif defined(OI_MIXED) && defined(MODULE_SLAVE)
#define Mixed MixedSlave
#include "BusCtrlSlave_AOut.h"
#include "BusCtrlSlave_AInLV.h"
#include "BusCtrlSlave_DIn.h"
#include "BusCtrlSlave_DOut.h"
#elif defined(MODULE_MASTER) 
#define Mixed MixedBusCtrl
#include "BusCtrlCmd_AOut.h"
#include "BusCtrlCmd_AInLV.h"
#include "BusCtrlCmd_DIn.h"
#include "BusCtrlCmd_DOut.h"
#endif

#if defined(OI_MIXED)

class MixedCLI : public CLI_AOut, public CLI_AInLV, public CLI_DIn, public CLI_DOut
{
public:

    static int init(void);
};

class MixedStandalone : public ModuleStandalone, public AnalogOutputs, public AnalogInputsLV, public DigitalOutputs, public DigitalInputs
{
public:

    static int init(void);
};
#endif

#if defined(OI_MIXED) && defined(MODULE_SLAVE)

class MixedSlave : public BusCtrlSlave, public MixedStandalone, public BusCtrlSlave_AOut, public BusCtrlSlave_AInLV, public BusCtrlSlave_DIn, public BusCtrlSlave_DOut
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class MixedBusCtrl : public BusCtrlCmd, public BusCtrlCmd_AOut, public BusCtrlCmd_AInLV, public BusCtrlCmd_DIn, public BusCtrlCmd_DOut
{
public:

    MixedBusCtrl(uint32_t sn = 0) : 
        BusCtrlCmd(TYPE_OI_MIXED , sn),
        BusCtrlCmd_AOut(this),
        BusCtrlCmd_AInLV(this),
        BusCtrlCmd_DIn(this),
        BusCtrlCmd_DOut(this) {}
};
#endif


