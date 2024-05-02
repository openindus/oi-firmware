/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"

class ModuleCLI
{
public:

    static int init(void);

private:

    static int _registerRestart(void);
    static int _registerSetBoardInfo(void);
    static int _registerGetBoardInfo(void);
    static int _registerLed(void);
    static int _registerLog(void);
    static int _registerReadId(void);
    static int _registerWriteSync(void);
    static int _registerReadSync(void);
    static int _registerBusPower(void);
};
