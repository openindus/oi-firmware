/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputsCmdHandler.h
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

#include "SlaveController.h"
#include "DigitalInputs.h"

class DigitalInputsCmdHandler
{
public:

    static int init();

private:

    static IsrCallback_t _isrCallback[DIN_MAX];
};

#endif