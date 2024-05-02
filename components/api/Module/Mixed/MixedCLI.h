/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "MixedStandalone.h"
#include "DigitalInputsCLI.h"
#include "DigitalOutputsCLI.h"
#include "AnalogInputsLVCLI.h"
#include "AnalogOutputsCLI.h"

#if defined(OI_MIXED)

class MixedCLI : public AnalogOutputsCLI, public AnalogInputsLVCLI, public DigitalInputsCLI, public DigitalOutputsCLI
{
public:

    static int init(void);
};

#endif
