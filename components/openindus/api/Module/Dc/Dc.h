/**
 * @file Dc.h
 * @brief DC Motor Module
 * @author OpenIndus Team
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"
#include "DcConfig.h"
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
        MotorDcPidCtrlCmd(this)
    {
        for (int i = 0; i < DC_ENCODER_MAX; i++) {
            encoder[i] = new EncoderCmd(this, i);
        }
    }
    
    EncoderCmd *encoder[DC_ENCODER_MAX];
};
#endif

