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
#include "Module.h"
#include "Core.h"
#include "Discrete.h"
#include "Mixed.h"
#include "Stepper.h"
#include "Relay.h"
#include "AnalogLs.h"
#include "Brushless.h"

void setup(void);
void loop(void);

class System
{
public:

    static void init(void);

private:

    static void _mainTask(void *pvParameters);

};