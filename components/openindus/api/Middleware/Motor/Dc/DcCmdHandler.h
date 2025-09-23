/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcCmdHandler.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#include "Slave.h"
#include "MotorDc.h"

#if defined(CONFIG_MODULE_SLAVE)

class DcCmdHandler
{
public:
    static int init();
};

#endif