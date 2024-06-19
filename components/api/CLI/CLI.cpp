/**
 * @file CLI.cpp
 * @brief Command line interface
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "CLI.h"

int CLI::init(void)
{
    int ret = 0;

#if defined(OI_ANALOG_LS)
    ret |= _registerAnalogLSCmd();
#endif

    return ret;
}