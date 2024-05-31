/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CoreStandalone.h
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"
#include "DigitalOutputsExp.h"
#include "DigitalInputs.h"
#include "AnalogInputsHV.h"
#include "RTClock.h"
#include "CAN.h"
#include "RS.h"

#if defined(OI_CORE)

class CoreStandalone : 
    public ModuleStandalone, 
    public DigitalInputs, 
    public DigitalOutputsExp, 
    public AnalogInputsHV
{
public:

    static int init(void);

    /* External bus */
    static CAN can;
    static RS rs;

private:

    static ioex_device_t *_ioex;
    static void _controlTask(void *pvParameters);
};

#endif