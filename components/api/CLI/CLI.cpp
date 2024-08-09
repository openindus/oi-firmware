/**
 * @file CLI.cpp
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"

int CLI::init(void)
{
    int ret = 0;

    ret |= _registerBoardCmd();
    ret |= _registerLedCmd();

#if !defined(MODULE_STANDALONE)
    ret |= _registerBusCmd();
#endif

#if defined(OI_CORE)
    ret |= _registerDigitalInputsCmd();
    ret |= _registerDigitalOutputsCmd();
    ret |= _registerAnalogInputsHVCmd();
    ret |= _registerCoreCmd();
#endif

#if defined(OI_ANALOG_LS)
    ret |= _registerAnalogInputsLSCmd();
#endif

#if defined(OI_STEPPER) || defined(OI_STEPPER_VE)
    ret |= _registerDigitalInputsCmd();
    ret |= _registerStepperCmd();
    ret |= _registerStepperParamCmd();
#endif

#if defined(OI_DC)
    ret |= _registerDigitalInputsCmd();
    ret |= _registerDcCmd();
#endif

#if defined(OI_RELAY_HP) || defined(OI_RELAY_LP)
    ret |= _registerRelayCmd();
#endif

#if defined(OI_DISCRETE)
    ret |= _registerDigitalInputsCmd();
    ret |= _registerDigitalOutputsCmd();
    ret |= _registerAnalogInputsHVCmd();
#endif

#if defined(OI_MIXED)
    ret |= _registerDigitalInputsCmd();
    ret |= _registerDigitalOutputsCmd();
    ret |= _registerAnalogInputsLVCmd();
    ret |= _registerAnalogOutputsCmd();
#endif

#if defined(MODULE_MASTER) || defined(MODULE_SLAVE)
    ret |= _registerControllerCmd();
#endif

    return ret;
}