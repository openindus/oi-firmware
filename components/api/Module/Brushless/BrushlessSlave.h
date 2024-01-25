/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessStandalone.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "BrushlessStandalone.h"
#include "ModuleSlave.h"

#if defined(CONFIG_BRUSHLESS)

class BrushlessSlave : public ModuleSlave
{
public:

    static void init(void);

private:

};

#endif