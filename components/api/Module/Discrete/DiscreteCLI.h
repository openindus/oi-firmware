/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "DiscreteStandalone.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"
#include "AnalogInputsHVCLI.h"

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

class DiscreteCLI : public AnalogInputsHVCLI, public DigitalInputsCLI, public DigitalOutputsCLI
{
public:

    static int init(void);
};

#endif
