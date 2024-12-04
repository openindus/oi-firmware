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
#include "RawSensor.h"
#include "RTD.h"
#include "Thermocouple.h"
#include "StrainGauge.h"

typedef enum {
    SAMPLE_400_MS = 0,
    SAMPLE_200_MS,
    SAMPLE_100_MS,
    SAMPLE_60_MS,
    SAMPLE_50_MS, // Default value
    SAMPLE_20_MS,
    SAMPLE_16_6_MS,
    SAMPLE_10_MS,
    SAMPLE_5_MS,
    SAMPLE_2_5_MS,
    SAMPLE_1_25_MS,
    SAMPLE_1_MS
} AcquisitionDuration_e;

class AnalogInputsLS
{

public: 

    /* List of Sensors */
    static std::vector<Sensor *> sensors;

    /**
     * @brief Set the Acquisition Time 
     * By default the adc reading start right after the 
     * @param duration 
     * @return int 
     */
    static void setAcquisitionTime(AcquisitionDuration_e duration);
    
    /**
     * @brief Set the Stabilization Time 
     * By default the adc reading start right after a call to read() function.
     * But if your sensor need some time to stabilize (sensor with an integrated RC filter for example),
     * you can set up a delay in milliseconds before the reading starts.
     * 
     * @param duration
     */
    static void setStabilizationTime(int t);

    /**
     * @brief Add a new sensor
     * 
     * @param type [RAW_SENSOR; RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE[B|E|J|K|N|R|S|T]; STRAIN_GAUGE]
     * @param ainPins an array initialised with AIN_NULL for unused members, that contains the pins set by the user
     * @return int the index of the added element (first call to this function will return 0, second call 1, ...).
     *         return -1 in case of error
     */
    static int addSensor(Sensor_Type_e type, std::array<AIn_Num_e, 4> ainPins);

    // static inline int addStrainGauge(const std::array<AIn_Num_t, 2>& signalInputs, 
    //                                  const std::array<AIn_Num_t, 2>& excitationInputs) {
    //     std::vector<AIn_Num_t> inputs;
    //     inputs.reserve(signalInputs.size() + excitationInputs.size());
    //     inputs.insert(inputs.end(), signalInputs.begin(), signalInputs.end());
    //     inputs.insert(inputs.end(), excitationInputs.begin(), excitationInputs.end());
    //     return addSensor(STRAIN_GAUGE, inputs);
    // }

    /* Assessors */

    static inline ADS114S0X* getAdcDevice(void) {
        return _adc;
    }

    static inline Multiplexer* getHighSideMux(void) {
        return _highSideMux;
    }

    static inline Multiplexer* getLowSideMux(void) {
        return _lowSideMux;
    }

    static inline Digipot* getDigipot(void) {
        return _digipot;
    }

protected:

    /* ADC */
    static ADS114S0X* _adc;

    /* Excitation */
    static Multiplexer* _highSideMux;
    static Multiplexer* _lowSideMux;
    static Digipot* _digipot;

    static int _init(void);
};


