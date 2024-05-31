/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * @file CLI_Core.h
 * @brief OpenIndus console
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "CoreStandalone.h"
#include "CLI_DIn.h"
#include "CLI_DOut.h"
#include "CLI_AInHV.h"

#if defined(OI_CORE)

class CLI_Core : public CLI_AInHV, public CLI_DIn, public CLI_DOut
{
public:

    static int init(void);

private:

    static int _registerDate(void);
};

#endif
