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

    static void init(void);

private:

    static void _registerRestart(void);
    static void _registerSetBoardInfo(void);
    static void _registerGetBoardInfo(void);
    static void _registerLed(void);
    static void _registerLog(void);
    static void _registerReadId(void);
    static void _registerWriteSync(void);
    static void _registerReadSync(void);
};
