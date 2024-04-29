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

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "AnalogOutputsControl.h"
#include "AnalogInputsLVControl.h"

#if !defined(OI_MIXED)

class MixedControl : public ModuleControl
{
public:

    MixedControl(int sn = 0) : 
        ModuleControl(sn),
        AnalogOutputsControl(),
        AnalogInputsLVControl(),
        _isrCallback{NULL} {}

};

#endif