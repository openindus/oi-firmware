/**
 * @file AnalogInputsLS.h
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Digipot.h"
#include "Sensor.h"
#include "RTD.h"
#include "Thermocouple.h"
#include "StrainGauge.h"

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
protected:

    /* ADC */
    static ADS114S0X* _adc;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    static Digipot* _digipot;

    static int _init(void);


public: 

    /* Sensors */

    static std::vector<RTD> rtd;
    static std::vector<Thermocouple> tc;
    static std::vector<StrainGauge> sg;

    static int setConversionTime(uint32_t t);

    static int addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns);

    static inline int addRtd2wire(const std::array<AIn_Num_t, 2>& inputs) {
        return addSensor(RTD_TWO_WIRE, std::vector<AIn_Num_t>(inputs.begin(), inputs.end()));
    }

    static inline int addRtd3Wire(const std::array<AIn_Num_t, 3>& inputs) {
        return addSensor(RTD_THREE_WIRE, std::vector<AIn_Num_t>(inputs.begin(), inputs.end()));
    }

    static inline int addThermocouple(const std::array<AIn_Num_t, 2>& inputs) {
        return addSensor(THERMOCOUPLE, std::vector<AIn_Num_t>(inputs.begin(), inputs.end()));
    }

    static inline int addStrainGauge(const std::array<AIn_Num_t, 2>& signalInputs, 
                                     const std::array<AIn_Num_t, 2>& excitationInputs) {
        std::vector<AIn_Num_t> inputs;
        inputs.reserve(signalInputs.size() + excitationInputs.size());
        inputs.insert(inputs.end(), signalInputs.begin(), signalInputs.end());
        inputs.insert(inputs.end(), excitationInputs.begin(), excitationInputs.end());
        return addSensor(STRAIN_GAUGE, inputs);
    }

    /* Assessors */

    static inline ADS114S0X* getAdcDevice(void) {
        return _adc;
    }

    static inline Multiplexer* getHighSideMux(void) {
        return _highSideMux;
    }

    static inline Multiplexer* getLowSideMux(void) {
        return _highSideMux;
    }

    static inline Digipot* getDigipot(void) {
        return _digipot;
    }
};