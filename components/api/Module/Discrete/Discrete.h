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
#include "CLI_DIn.h"
#include "CLI_DOut.h"
#include "CLI_AInHV.h"
#include "ControllerSlave.h"
#include "ResponseDIn.h"
#include "ResponseDOut.h"
#include "ResponseAInHV.h"
#include "Controller.h"
#include "CommandDIn.h"
#include "CommandDOut.h"
#include "CommandAInHV.h"

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
    public CommandDIn, 
    public CommandDOut, 
    public CommandAInHV
{
public:

    Discrete(uint32_t sn = 0) : 
        Controller(TYPE_OI_DISCRETE, sn),
        CommandDIn(this),
        CommandDOut(this),
        CommandAInHV(this) {}
};

#endif