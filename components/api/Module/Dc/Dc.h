/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Dc.h
 * @brief Functions for dc module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "DcPinout.h"
#include "Module.h"
#include "Controller.h"
#include "ControllerSlave.h"
#include "DigitalInputs.h"
#include "DigitalInputsCmd.h"
#include "DigitalInputsRsp.h"
#include "MotorDc.h"
#include "DcRsp.h"
#include "DcCmd.h"

#if defined(OI_DC)

class Dc : 
    public Module, 
    public DigitalInputs, 
    public MotorDc
{
public:

    static int init(void);
};

#elif defined(MODULE_MASTER)

class Dc : 
    public Controller, 
    public DigitalInputsCmd, 
    public DcCmd
{
public:

    Dc(uint32_t sn = 0) : 
        Controller(TYPE_OI_DC, sn),
        DigitalInputsCmd(this),
        DcCmd(this) {}
};
#endif

