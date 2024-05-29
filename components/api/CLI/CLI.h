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

    static inline int registerCommands(void) {
        int err = 0;
        err |= _registerRestartCmd();
        err |= _registerLogCmd();
        err |= _registerLedCmd();
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
        err |= _registerReadIdCmd();
        err |= _registerWriteSyncCmd();
        err |= _registerReadSyncCmd();
        err |= _registerBusPowerCmd();
        err |= _registerCANWriteCmd();
        err |= _registerCANReadCmd();
        err |= _registerRSWriteCmd();
        err |= _registerRSReadCmd();
#endif
        return err;
    }

private:

    static int _registerRestartCmd(void);
    static int _registerLogCmd(void);

    /* Led */
    static int _registerLedCmd(void);

    /* Module (Master/Standalone) */
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

    /* Bus */
#if !defined(MODULE_STANDALONE)
    static int _registerReadIdCmd(void);
    static int _registerWriteSyncCmd(void);
    static int _registerReadSyncCmd(void);
    static int _registerBusPowerCmd(void);
    static int _registerCANWriteCmd(void);
    static int _registerCANReadCmd(void);
    static int _registerRSWriteCmd(void);
    static int _registerRSReadCmd(void);
#endif

};