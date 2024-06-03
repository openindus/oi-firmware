/**
 * @file CLI_Board.h
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Board.h"

class CLI_Board
{
public: 

    static inline int init(void) {
        int err = 0;
        err |= _registerSetBoardInfoCmd();
        err |= _registerGetBoardInfoCmd();
        err |= _registerRestartCmd();
        err |= _registerLogCmd();
        return err;
    }

private:

    static int _registerSetBoardInfoCmd(void);
    static int _registerGetBoardInfoCmd(void);
    static int _registerRestartCmd(void);
    static int _registerLogCmd(void);

};