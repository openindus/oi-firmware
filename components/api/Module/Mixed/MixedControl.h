/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedControl.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "ModuleControl.h"
#include "DigitalInputsControl.h"
#include "DigitalOutputsControl.h"
#include "AnalogInputsLVControl.h"
#include "AnalogOutputsControl.h"

class MixedControl : public ModuleControl, public AnalogOutputsControl, public AnalogInputsLVControl, public DigitalInputsControl, public DigitalOutputsControl
{
public:

    MixedControl(int sn = 0) : 
        ModuleControl(sn),
        AnalogOutputsControl(this),
        AnalogInputsLVControl(this),
        DigitalInputsControl(this),
        DigitalOutputsControl(this) {}
};

#endif