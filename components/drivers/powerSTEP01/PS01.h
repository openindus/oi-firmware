/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "PS01_Global.h"
#include "PS01_Utils.h"
#include "PS01_Param.h"
#include "PS01_Register.h"
#include "PS01_Cmd.h"
#include "PS01_Hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

void PS01_Init(PS01_Hal_Config_t* config, PS01_Param_t* param);

#ifdef __cplusplus
}
#endif