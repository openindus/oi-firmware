/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DcRsp.h
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_SLAVE)

#include "ControllerSlave.h"
#include "MotorDc.h"

class DcRsp
{
public:

    static int init();
};

#endif