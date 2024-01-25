/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ConsoleMaster.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"
#include "ModuleMaster.h"

#if defined(CONFIG_CORE)

class ConsoleMaster
{
public:

    static void registerCli(void);

private:

    static void _registerProgram(void);
    static void _registerPing(void);

};

#endif
