/**
 * @file AnalogLsStandalone.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Module.h"
#include "AnalogLsInterface.h"
#include "AnalogLsPinout.h"

#if defined(OI_ANALOG_LS)

class AnalogLsStandalone : public ModuleStandalone, public AnalogLsInterface
{
public:

    static int init(void);

};

#endif