/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CLI_DOut.h
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
#include "DigitalOutputsExp.h"

class CLI_DOut : public DigitalOutputs
{
public:

    static inline int init(void) {
        int err = 0;
        err |= _registerDigitalWrite();
#if !defined(OI_CORE)
        err |= _registerGetOutputCurrent();
#endif
        err |= _registerOuputIsOvercurrent();
        return err;
    };

private:

    static int _registerDigitalWrite();
#if !defined(OI_CORE)
    static int _registerGetOutputCurrent();
#endif
    static int _registerOuputIsOvercurrent();
};