/**
 * @file Discrete.h
 * @brief Discrete
 * @author
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"
#include "DiscretePinout.h"
#include "DigitalInputs.h"
#include "DigitalOutputs.h"
#include "AnalogInputsHV.h"
#include "SlaveController.h"
#include "DigitalInputsCmdHandler.h"
#include "DigitalOutputsCmdHandler.h"
#include "AnalogInputsHVCmdHandler.h"
#include "Controller.h"
#include "DigitalInputsCmd.h"
#include "DigitalOutputsCmd.h"
#include "AnalogInputsHVCmd.h"

#if (defined(OI_DISCRETE) || defined(OI_DISCRETE_VE))

class Discrete : public Module, 
    public DigitalInputs, 
    public DigitalOutputs, 
    public AnalogInputsHV
{
public:

    static int init(void);

};

#elif defined(MODULE_MASTER) 

class Discrete : 
    public Controller, 
    public DigitalInputsCmd, 
    public DigitalOutputsCmd, 
    public AnalogInputsHVCmd
{
public:

    Discrete(uint32_t sn = 0) : 
        Controller(TYPE_OI_DISCRETE, sn),
        DigitalInputsCmd(this),
        DigitalOutputsCmd(this),
        AnalogInputsHVCmd(this) {}
};

#endif