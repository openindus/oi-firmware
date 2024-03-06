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

/* Module */
#include "ModuleStandalone.h"
#include "ModuleSlave.h"
#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "ModuleCLI.h"
#include "ModuleMasterCLI.h"

/* Core */
#include "CoreStandalone.h"
#include "CoreMaster.h"
#include "CoreCLI.h"

/* Discrete */
#include "DiscreteStandalone.h"
#include "DiscreteSlave.h"
#include "DiscreteControl.h"
#include "DiscreteCLI.h"

/* Mixed */
#include "MixedStandalone.h"
#include "MixedSlave.h"
#include "MixedControl.h"
#include "MixedCLI.h"

/* Stepper */
#include "StepperStandalone.h"
#include "StepperSlave.h"
#include "StepperControl.h"
#include "StepperCLI.h"
#include "StepperParamCLI.h"

/* Relay */
#include "RelayStandalone.h"
#include "RelaySlave.h"
#include "RelayControl.h"

/* Brushless */
#include "BrushlessStandalone.h"
#include "BrushlessSlave.h"
#include "BrushlessControl.h"

/* AnalogLs*/
#include "AnaloglsStandalone.h"
#include "AnaloglsSlave.h"
#include "AnaloglsControl.h"

void setup(void);
void loop(void);

class System
{
public:

    static void init(void);

private:

    static void _mainTask(void *pvParameters);

};