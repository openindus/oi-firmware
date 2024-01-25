/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of  file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteSlave.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once 

#include "AnaloglsStandalone.h"
#include "ModuleSlave.h"

#if defined(CONFIG_ANALOG_LS)

class AnaloglsSlave : public AnaloglsStandalone, public ModuleSlave
{
public: 

    static void init(void);

private:

};
#endif 

