/**
 * @file Dc.h
 * @brief DC Motor Module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

#define DC_ENCODER_MAX 2

#if defined(CONFIG_OI_DC)

#include "Module.h"
#include "Slave.h"
#include "DigitalInputs.h"
#include "MotorDc.h"
#include "Encoder.h"
#include "DigitalInputsCmdHandler.h"
#include "MotorDcPidCtrlCmdHandler.h"
#include "EncoderCmdHandler.h"

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

#include "ModuleControl.h"
#include "DigitalInputsCmd.h"
#include "MotorDcPidCtrlCmd.h"
#include "EncoderCmd.h"

class Dc : 
    public ModuleControl, 
    public DigitalInputsCmd, 
    public MotorDcPidCtrlCmd
{
public:
    Dc(uint32_t sn = 0) : 
        ModuleControl(TYPE_OI_DC, sn),
        DigitalInputsCmd(this),
        MotorDcPidCtrlCmd(this)
    {
        for (int i = 0; i < DC_ENCODER_MAX; i++) {
            encoder[i] = new EncoderCmd(this, i);
        }
    }
    
    EncoderCmd *encoder[DC_ENCODER_MAX];
};
#endif

