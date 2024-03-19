/**
 * @file AnalogLs.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "AnalogLsStandalone.h"
#include "AnalogLsSlave.h"
#include "AnalogLsControl.h"

#if defined(OI_ANALOG_LS) && defined(MODULE_STANDALONE)
#define Analogls AnaloglsStandalone
#elif defined(OI_ANALOG_LS) && !defined(MODULE_STANDALONE)
#define Analogls AnaloglsSlave
#else 
#define Analogls AnaloglsControl
#endif
