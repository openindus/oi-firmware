/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file System.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ConsoleCore.h"
#include "ConsoleDiscrete.h"
#include "ConsoleMaster.h"
#include "ConsoleModule.h"
#include "ConsoleStepper.h"
#include "MotorStepperParamCLI.h"
#include "Core.h"
#include "Discrete.h"
#include "Stepper.h"
#include "MixedStandalone.h"
#include "MixedSlave.h"
#include "MixedControl.h"
#include "MixedCLI.h"
#include "Relay.h"
#include "Brushless.h"
#include "Analogls.h"

void setup(void);
void loop(void);

class System
{
public:

    static void init(void);

private:

    static void _mainTask(void *pvParameters);

};