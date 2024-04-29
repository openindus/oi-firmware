/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteStandalone.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

class DiscreteStandalone : public ModuleStandalone, public DigitalInputs, public DigitalOutputs, public AnalogInputsHV
{
public:

    static int init(void);
};

#endif