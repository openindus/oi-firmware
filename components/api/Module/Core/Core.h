/**
 * @file Core.h
 * @brief Module Core
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"
#include "DigitalOutputsExp.h"
#include "DigitalInputsExp.h"
#include "AnalogInputsHV.h"
#include "RTClock.h"
#include "CAN.h"
#include "RS.h"

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

private:

    static ioex_device_t *_ioex;
    static void _controlTask(void *pvParameters);
};

#endif