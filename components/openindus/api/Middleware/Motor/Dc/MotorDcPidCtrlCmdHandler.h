/**
 * @file MotorDcPidCtrlCmdHandler.h
 * @brief MotorDcPidCtrlCmdHandler class — slave-side bus callback handler for DC PID position control
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "MotorDcCmdHandler.h"
#include "Slave.h"
#include "MotorDcPidCtrl.h"

#if defined(CONFIG_MODULE_SLAVE)

class MotorDcPidCtrlCmdHandler : public MotorDcCmdHandler
{
public:
    /**
     * @brief Register all DC PID motor callbacks with the slave bus.
     * @return int 0 on success, non-zero on error
     */
    static int init(void);
};

#endif
