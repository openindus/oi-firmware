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

#if defined(OI_DISCRETE) || defined(OI_DISCRETE_VE)

class DiscreteCLI
{
public:

    static void init(void);

private:

    static void _registerDigitalWrite(void);
    static void _registerDigitalRead(void);
    static void _registerDigitalGetCurrent(void);
    static void _registerAnalogInputRead(void);
    static void _registerSetAnalogCoeffs(void);

};

#endif
