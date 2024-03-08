/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModuleMasterCLI.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"
#include "ModuleMaster.h"

#if defined(MODULE_MASTER)

class ModuleMasterCLI
{
public:

    static void init(void);

private:

    static void _registerProgram(void);
    static void _registerPing(void);
    static void _registerAutoId(void);

};

#endif
