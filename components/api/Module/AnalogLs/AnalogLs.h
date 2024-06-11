/**
 * @file AnalogLs.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Module.h"

#if defined(OI_ANALOG_LS)

#include "RTD.h"
#include "Thermocouple.h"
#include "WheastoneBridge.h"

typedef enum {
    EANA_A_P = 0,
    EANA_A_N,
    EANA_B_P,
    EANA_B_N,
    EANA_C_P,
    EANA_C_N,
    EANA_D_P,
    EANA_D_N,
    EANA_E_P,
    EANA_E_N,
    EANA_MAX
} EAna_num_t;

class AnalogLsStandalone:
    public ModuleStandalone
{
public:

    static int init(void);

    // static RTD rtd;

};

#endif

#if (OI_ANALOG_LS) // && defined(MODULE_STANDALONE)
class AnalogLs : public AnalogLsStandalone {};
#endif