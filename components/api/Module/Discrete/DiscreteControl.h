/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "AnalogInputsLVControl.h"
#include "DigitalInputsControl.h"
#include "DigitalOutputsControl.h"

#if defined(MODULE_MASTER)

class DiscreteControl : public ModuleControl, public AnalogInputsHVControl, public DigitalInputsControl, public DigitalOutputsControl
{
public:

    MixedControl(int sn = 0) : 
        ModuleControl(sn),
        AnalogInputsHVControl(),
        DigitalInputsControl(),
        DigitalOutputsControl() {}
};

#endif