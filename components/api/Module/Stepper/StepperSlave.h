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

#if (defined(OI_STEPPER) || defined(OI_STEPPER_VE)) && defined(MODULE_SLAVE)

#include "StepperStandalone.h"
#include "ModuleSlave.h"
#include "DigitalInputsSlave.h"

class StepperSlave : public StepperStandalone, public ModuleSlave, public DigitalInputsSlave
{
public:

    static int init(void);

private:

    static IsrCallback_t _isrCallback[4];

};

#endif
