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

    static inline int init(void) {
        int err = 0;
        err |= _registerRestartCmd();
        err |= _registerLogCmd();
        return err;
    }

private:

    static int _registerRestartCmd(void);
    static int _registerLogCmd(void);

};