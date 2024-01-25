/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperSlave.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "StepperStandalone.h"
#include "ModuleSlave.h"

#if defined(CONFIG_STEPPER) || defined(CONFIG_STEPPER_VE)

class StepperSlave : public StepperStandalone, public ModuleSlave
{
public:

    static void init(void);

private:

    static IsrCallback_t _isrCallback[4];

};

#endif
