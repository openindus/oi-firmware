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
#include "AnalogOutputsCLI.h"
#include "AnalogInputsLVCLI.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"
#endif

#if defined(OI_MIXED) && defined(MODULE_STANDALONE)
#define Mixed MixedStandalone
#elif defined(OI_MIXED) && defined(MODULE_SLAVE)
#define Mixed MixedSlave
#include "AnalogOutputsSlave.h"
#include "AnalogInputsLVSlave.h"
#include "DigitalInputsSlave.h"
#include "DigitalOutputsSlave.h"
#elif defined(MODULE_MASTER) 
#define Mixed MixedControl
#include "AnalogOutputsControl.h"
#include "AnalogInputsLVControl.h"
#include "DigitalInputsControl.h"
#include "DigitalOutputsControl.h"
#endif

#if defined(OI_MIXED)

class MixedCLI : public AnalogOutputsCLI, public AnalogInputsLVCLI, public DigitalInputsCLI, public DigitalOutputsCLI
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

class MixedSlave : public ModuleSlave, public MixedStandalone, public AnalogOutputsSlave, public AnalogInputsLVSlave, public DigitalInputsSlave, public DigitalOutputsSlave
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class MixedControl : public ModuleControl, public AnalogOutputsControl, public AnalogInputsLVControl, public DigitalInputsControl, public DigitalOutputsControl
{
public:

    MixedControl(int sn = 0) : 
        ModuleControl(TYPE_OI_MIXED , sn),
        AnalogOutputsControl(this),
        AnalogInputsLVControl(this),
        DigitalInputsControl(this),
        DigitalOutputsControl(this) {}
};
#endif


