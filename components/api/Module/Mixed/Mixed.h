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
#include "ControlSlave_AOut.h"
#include "ControlSlave_AInLV.h"
#include "ControlSlave_DIn.h"
#include "ControlSlave_DOut.h"
#elif defined(MODULE_MASTER) 
#define Mixed MixedControl
#include "ControlCmd_AOut.h"
#include "ControlCmd_AInLV.h"
#include "ControlCmd_DIn.h"
#include "ControlCmd_DOut.h"
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

class MixedSlave : public ControlSlave, public MixedStandalone, public ControlSlave_AOut, public ControlSlave_AInLV, public ControlSlave_DIn, public ControlSlave_DOut
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class MixedControl : public ControlCmd, public ControlCmd_AOut, public ControlCmd_AInLV, public ControlCmd_DIn, public ControlCmd_DOut
{
public:

    MixedControl(uint32_t sn = 0) : 
        ControlCmd(TYPE_OI_MIXED , sn),
        ControlCmd_AOut(this),
        ControlCmd_AInLV(this),
        ControlCmd_DIn(this),
        ControlCmd_DOut(this) {}
};
#endif


