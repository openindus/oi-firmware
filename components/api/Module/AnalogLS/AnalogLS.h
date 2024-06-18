/**
 * @file AnalogLS.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"
#include "AnalogLSConfig.h"
#include "AnalogInputsLS.h"
#include "RTD.h"
#include "Thermocouple.h"
#include "StrainGauge.h"

#if (OI_ANALOG_LS)

class AnalogLS:
    public Module,
    public AnalogInputsLS
{
public:

    static int init(void);

    static std::vector<RTD> rtd;

};

#else

class AnalogLS
{

};

#endif