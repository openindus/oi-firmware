/**
 * @file AnalogLsStandalone.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ModuleStandalone.h"
#include "AnalogLs.h"
#include "AnalogLsPinout.h"

#if defined(OI_ANALOG_LS)

class AnalogLsStandalone : public ModuleStandalone, public AnalogLs
{
public:

    static int init(void);

};

#endif