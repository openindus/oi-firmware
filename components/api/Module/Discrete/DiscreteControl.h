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

#if defined(MODULE_MASTER)

#include "ModuleControl.h"
#include "DigitalInputsControl.h"
#include "DigitalOutputsControl.h"
#include "AnalogInputsHVControl.h"

class DiscreteControl : public ModuleControl, public DigitalInputsControl, public DigitalOutputsControl, public AnalogInputsHVControl
{
public:

    DiscreteControl(int sn = 0) : 
        ModuleControl(sn),
        DigitalInputsControl(this),
        DigitalOutputsControl(this),
        AnalogInputsHVControl(this) {}
};

#endif