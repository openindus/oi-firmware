/**
 * @file Core.h
 * @brief Module Core
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "Module.h"

#ifndef OI_CORE_PLUS

#include "DigitalOutputsExp.h"
#include "DigitalInputsExp.h"
#include "AnalogInputsHV.h"
#include "RTClock.h"
#include "CAN.h"
#include "RS.h"
#include "Modem.hpp"

#if defined(OI_CORE)

class Core : 
    public Module, 
    public DigitalInputsExp, 
    public DigitalOutputsExp, 
    public AnalogInputsHV
{
public:

    static int init(void);

    /* External bus */
    static CAN can;
    static RS rs;
    static RTClock rtc;

    static Modem *modem;

private:

    static ioex_device_t *_ioex;
    static void _moduleTask(void *pvParameters);
};

#endif

#endif