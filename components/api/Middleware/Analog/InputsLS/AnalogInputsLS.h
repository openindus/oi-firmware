/**
 * @file AnalogInputsLS.h
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ADS114S0X.h"
#include "Multiplexer.h"
#include "RTD.h"

typedef enum {
    AINLS_A_P = 0,
    AINLS_A_N,
    AINLS_B_P,
    AINLS_B_N,
    AINLS_C_P,
    AINLS_C_N,
    AINLS_D_P,
    AINLS_D_N,
    AINLS_E_P,
    AINLS_E_N,
    AINLS_MAX
} AInLS_Num_t;

class AnalogInputsLS
{
public: 

    static std::vector<RTD> rtd;

    static int addRTD(AInLS_Num_t ainP, AInLS_Num_t ainN);
    static int addRTD(AInLS_Num_t ainP0, AInLS_Num_t ainN0, AInLS_Num_t ainN1);

protected:

    static int init(void);

    /* ADC */
    static ADS114S0X* _adc;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    // static Digipot* _digipot;

};