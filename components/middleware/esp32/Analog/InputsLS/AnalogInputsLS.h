/**
 * @file AnalogInputsLS.h
 * @brief Analog inputs low signal
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "_ADS114S0X.h"
#include "Multiplexer.h"
#include "Digipot.h"
#include "Sensor.h"
#include "RawSensor.h"
#include "RTD.h"
#include "Thermocouple.h"
#include "StrainGauge.h"

class AnalogInputsLS
{

public: 

    /* List of Sensors */
    static std::vector<Sensor *> sensors;

    /**
     * @brief Add a new sensor
     * 
     * @param type [RAW_SENSOR; RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE[B|E|J|K|N|R|S|T]; STRAIN_GAUGE]
     * @param ainPins a vector that contains the pins set by the user
     * @return int the index of the added element (first call to this function will return 0, second call 1, ...).
     *         return -1 in case of error
     */
    static int addSensor(Sensor_Type_e type, std::vector<AIn_Num_e> ainPins);

    /**
     * @brief Lists all sensors
     *
     * @return nbr of sensors
     */
    static int list_sensors(void);

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


