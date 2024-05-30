/**
 * @file CLI.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "LedCLI.h"
#include "BusCLI.h"

class ModuleCLI : 
    public LedCLI,
    public BusCLI
{
public: 

    static inline int init(void) {
        int err = 0;
        err |= LedCLI::init();
        err |= BusCLI::init();
        err |= _registerSetBoardInfoCmd();
        err |= _registerGetBoardInfoCmd();
#if defined(MODULE_MASTER)
        err |= _registerPingCmd();
        err |= _registerProgramCmd();
        err |= _registerAutoIdCmd();
        err |= _registerDiscoverSlavesCmd();
        err |= _registerGetSlaveInfoCmd();
#endif
#if !defined(MODULE_STANDALONE)
        err |= _registerModuleStopCmd();
        err |= _registerModuleStartCmd();
        err |= _registerModuleGetStatusCmd();
#endif
        return err;
    }

private:

    static int _registerSetBoardInfoCmd(void);
    static int _registerGetBoardInfoCmd(void);
#if defined(MODULE_MASTER)
    static int _registerProgramCmd(void);
    static int _registerPingCmd(void);
    static int _registerAutoIdCmd(void);
    static int _registerDiscoverSlavesCmd(void);
    static int _registerGetSlaveInfoCmd(void);
#endif
    static int _registerModuleStopCmd(void);
    static int _registerModuleStartCmd(void);
    static int _registerModuleGetStatusCmd(void);

};