/**
 * @file CLI_Led.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class CLI_Led
{
public: 

    static inline int init(void) {
        int err = 0;
        err |= _registerLedCmd();
        return err;
    }

private:

    static int _registerLedCmd(void);

};