/**
 * @file CLI_Controller.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class CLI_Controller
{
public: 

    static inline int init(void) {
        int err = 0;
#if defined(MODULE_MASTER)
        err |= _registerPingCmd();
        err |= _registerProgramCmd();
        err |= _registerAutoIdCmd();
        err |= _registerDiscoverSlavesCmd();
        err |= _registerGetSlaveInfoCmd();
#endif
#if !defined(MODULE_STANDALONE)
        err |= _registerStopCmd();
        err |= _registerStartCmd();
        err |= _registerGetStatusCmd();
#endif
        return err;
    }

private:

#if defined(MODULE_MASTER)
    static int _registerProgramCmd(void);
    static int _registerPingCmd(void);
    static int _registerAutoIdCmd(void);
    static int _registerDiscoverSlavesCmd(void);
    static int _registerGetSlaveInfoCmd(void);
#endif
#if !defined(MODULE_STANDALONE)
    static int _registerStopCmd(void);
    static int _registerStartCmd(void);
    static int _registerGetStatusCmd(void);
#endif

};