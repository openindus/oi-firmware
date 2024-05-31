/**
 * @file CLI_Bus.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class CLI_Bus
{
public: 

    static inline int init(void) {
        int err = 0;
#if !defined(MODULE_STANDALONE)
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