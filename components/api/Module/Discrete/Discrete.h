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
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"
#include "AnalogInputsHVCLI.h"
#endif

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_STANDALONE)
#define Discrete DiscreteStandalone
#elif (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)) && defined(MODULE_SLAVE)
#define Discrete DiscreteSlave
#include "AnalogInputsHVSlave.h"
#include "DigitalInputsSlave.h"
#include "DigitalOutputsSlave.h"
#elif defined(MODULE_MASTER) 
#define Discrete DiscreteControl
#include "AnalogInputsHVControl.h"
#include "DigitalInputsControl.h"
#include "DigitalOutputsControl.h"
#endif


#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)
class DiscreteCLI : public AnalogInputsHVCLI, public DigitalInputsCLI, public DigitalOutputsCLI
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
class DiscreteSlave : public ModuleSlave, public DiscreteStandalone, public DigitalInputsSlave, public DigitalOutputsSlave, public AnalogInputsHVSlave
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER) 
class DiscreteControl : public ModuleControl, public DigitalInputsControl, public DigitalOutputsControl, public AnalogInputsHVControl
{
public:

    DiscreteControl(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_DISCRETE, sn),
        DigitalInputsControl(this),
        DigitalOutputsControl(this),
        AnalogInputsHVControl(this) {}
};
#endif