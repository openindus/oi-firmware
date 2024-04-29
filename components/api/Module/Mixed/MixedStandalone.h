/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedStandalone.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"
#include "MixedPinout.h"

#if defined(OI_MIXED)

class MixedStandalone : public ModuleStandalone, public AnalogOutputs, public AnalogInputsLV, public DigitalOutputs, public DigitalInputs
{
public:

    static int init(void);

};

#endif