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
#include "Sensor.h"
#include "RTD.h"

typedef enum {
    AIN_UNDEFINED = -1,
    AIN_A_P = 0,
    AIN_A_N,
    AIN_B_P,
    AIN_B_N,
    AIN_C_P,
    AIN_C_N,
    AIN_D_P,
    AIN_D_N,
    AIN_E_P,
    AIN_E_N,
    AIN_MAX
} AIn_Num_t;

class AnalogInputsLS
{
public: 

    static std::vector<RTD> rtd;

    static int addSensor(Sensor_Type_t sensor, AIn_Num_t aInP, AIn_Num_t aInN);
    static int addSensor(Sensor_Type_t sensor, AIn_Num_t aInP0, AIn_Num_t aInN0, AIn_Num_t aInN1);

protected:

    static int init(void);

    /* ADC */
    static ADS114S0X* _adc;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    // static Digipot* _digipot;

};