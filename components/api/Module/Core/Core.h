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
#include "CLI_Core.h"

#if defined(OI_CORE) && defined(MODULE_STANDALONE)
#define Core CoreStandalone
#elif defined(OI_CORE) && !defined(MODULE_STANDALONE)
#define Core CoreMaster
#endif

#if defined(OI_CORE)

class CoreStandalone : 
    public ModuleStandalone, 
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

#if !defined(MODULE_STANDALONE)

#include "ControlMaster.h"

class CoreMaster: public CoreStandalone, public ControlMaster
{
public:

    static inline int init(void) {
        int err = ControlMaster::init();
        err |= CoreStandalone::init();
        return err;
    };
};

#endif

#endif