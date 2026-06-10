/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Dc.h
 * @brief Callbacks for dc module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "DcPinout.h"
#include "Module.h"
#include "Slave.h"
#include "ModuleControl.h"
#include "DigitalInputs.h"
#include "DigitalInputsCmd.h"
#include "DigitalInputsCmdHandler.h"
#include "MotorDc.h"
#include "MotorDcPidCtrlCmdHandler.h"
#include "MotorDcPidCtrlCmd.h"
#include "DigitalInputsCLI.h"
#include "Encoder.h"
#include "EncoderCmd.h"
#include "EncoderCmdHandler.h"

#define DC_ENCODER_MAX 2

#if defined(CONFIG_OI_DC)

class Dc : 
    public Module, 
    public DigitalInputs, 
    public MotorDcPidCtrl
{
public:
    static int init(void);

    static Encoder *encoder[DC_ENCODER_MAX];
};

#elif defined(CONFIG_MODULE_MASTER)

class Dc : 
    public ModuleControl, 
    public DigitalInputsCmd, 
    public MotorDcPidCtrlCmd
{
public:
    Dc(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_DC, sn),
        DigitalInputsCmd(this),
        MotorDcPidCtrlCmd(this) {}
    
    EncoderCmd *encoder[DC_ENCODER_MAX];
};
#endif

