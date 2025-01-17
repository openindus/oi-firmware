/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperCmdHandler.h
 * @author AFL
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_SLAVE)

#include "ControllerSlave.h"
#include "MotorStepper.h"

class StepperCmdHandler
{
public:

    static int init();

private:

    static void _waitTask(void *pvParameters);
};

#endif