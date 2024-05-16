/**
 * @file AnalogLsControl.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Module.h"
#include "AnalogLsInterface.h"

#if defined(MODULE_MASTER)

class AnalogLsControl : public ModuleControl, public AnalogLsInterface
{
public:

    AnalogLsControl(int sn = 0) : ModuleControl(TYPE_OI_ANALOG_LS, sn) {}

};

#endif