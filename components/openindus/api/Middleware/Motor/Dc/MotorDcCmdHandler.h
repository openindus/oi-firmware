/**
 * @file MotorDcCmdHandler.h
 * @brief MotorDcCmdHandler class definition
 * @author OpenIndus Team
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Slave.h"
#include "MotorDc.h"

#if defined(CONFIG_MODULE_SLAVE)

class MotorDcCmdHandler
{
public:
    static int init(void);
};

#endif
