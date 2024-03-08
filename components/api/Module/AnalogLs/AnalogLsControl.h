/**
 * @file AnalogLsControl.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "AnalogLs.h"

#if !defined(OI_ANALOG_LS)

class AnalogLsControl : public ModuleControl, public AnalogLs
{
public:

    AnalogLsControl(int sn = 0) : ModuleControl(sn) {}

};

#endif