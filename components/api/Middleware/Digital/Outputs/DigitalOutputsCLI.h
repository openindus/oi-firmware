/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalOutputsCLI.h
 * @author CBA
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#include "Global.h"
#include "DigitalOutputs.h"

class DigitalOutputsCLI : public DigitalOutputs
{
public:
    static int init();

private:
    static int _registerDigitalWrite();
    static int _registerDigitalGetCurrent();
    static int _registerDigitalGetOverCurrentStatus();
};