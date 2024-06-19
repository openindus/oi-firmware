/**
 * @file CLI.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class CLI
{
public:

    static int init(void);

private:

    static int _registerBoardCmd(void);
    static int _registerLedCmd(void);

#if !defined(MODULE_STANDALONE)
    static int _registerBusCmd(void);
#endif

#if defined(OI_CORE)
    static int _registerDigitalOutputsCmd(void);
    static int _registerDigitalInputsCmd(void);
    static int _registerAnalogInputsHVCmd(void);
    static int _registerCoreCmd(void);
#endif

#if defined(OI_ANALOG_LS)
    static int _registerAnalogLSCmd(void);
#endif

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    static int _registerDigitalInputsCmd(void);
    static int _registerStepperCmd(void);
    static int _registerStepperParamCmd(void);
#endif

#if defined(OI_RELAY_HP) || defined(OI_RELAY_LP)
    static int _registerRelayCmd(void);
#endif

#if defined(OI_DISCRETE)
    static int _registerDigitalInputsCmd(void);
    static int _registerDigitalOutputsCmd(void);
    static int _registerAnalogInputsHVCmd(void);
#endif

#if defined(OI_MIXED)
    static int _registerDigitalInputsCmd(void);
    static int _registerDigitalOutputsCmd(void);
    static int _registerAnalogInputsLVCmd(void);
    static int _registerAnalogOutputsCmd(void);
#endif

#if defined(MODULE_MASTER) || defined(MODULE_SLAVE)
    static int _registerControllerCmd(void);
#endif

};
