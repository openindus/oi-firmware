/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLSCmdHandler.h
 * @author AFL
 * @version 0.1
 * @date 2024-10-01
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
 */

#pragma once

#if defined(MODULE_SLAVE)

#include "ControllerTypes.h"
#include "Slave.h"
#include "AnalogInputsLS.h"

class AnalogInputsLSCmdHandler
{
public:
    static int init(void);
};

#endif