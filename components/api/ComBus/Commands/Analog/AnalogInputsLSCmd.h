/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLSCmd.h
 * @author AFL
 * @version 0.1
 * @date 2024-09-24
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "ControllerMaster.h"
#include "AnalogInputsLS.h"

class RawSensorCmd
{

public:

    RawSensorCmd(Controller* control, uint8_t index);
    void setGain(Sensor_Gain_e gain);
    int16_t read(void);
    float readMillivolts(void);
    
private:

    Controller* _control;
    uint8_t _index;
    xQueueHandle _readEvent;
};

class RTDCmd
{

public:

    RTDCmd(Controller* control, uint8_t index);
    float readResistor(void);
    float readTemperature(void);
    
private:

    Controller* _control;
    uint8_t _index;
    xQueueHandle _readEvent;
};

class ThermocoupleCmd
{

public:

    ThermocoupleCmd(Controller* control, uint8_t index);
    float readMillivolts(void);
    float readTemperature(void);
    
private:

    Controller* _control;
    uint8_t _index;
    xQueueHandle _readEvent;
};

class StrainGaugeCmd
{

public:

    StrainGaugeCmd(Controller* control, uint8_t index);
    void setExcitationMode(StrainGauge_Excitation_e excitation);
    float read(void);
    
private:

    Controller* _control;
    uint8_t _index;
    xQueueHandle _readEvent;
};

class AnalogInputsLSCmd
{
public:

    /* List of RTDs */
    std::vector<RTDCmd> rtd;
    /* List of thermocouples */
    std::vector<ThermocoupleCmd> tc;
    /* List of strain gauge*/
    std::vector<StrainGaugeCmd> sg;
    /* List of raw sensors */
    std::vector<RawSensorCmd> raw;

    AnalogInputsLSCmd(Controller* control) : _control(control) {}

    /**
     * @brief Set the Acquisition Time 
     * By default the adc reading start right after the 
     * @param duration 
     * @return int 
     */
    void setAcquisitionTime(AcquisitionDuration_e duration);
    
    /**
     * @brief Set the Stabilization Time 
     * By default the adc reading start right after a call to read() function.
     * But if your sensor need some time to stabilize (sensor with an integrated RC filter for example),
     * you can set up a delay in milliseconds before the reading starts.
     * 
     * @param duration
     */
    void setStabilizationTime(int duration);
    
    /**
     * @brief Add a new sensor
     * 
     * @param type [RAW_SENSOR; RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE[B|E|J|K|N|R|S|T]; STRAIN_GAUGE]
     * @param aIns Analog Inputs (AIN_A_P to AIN_E_N)
     * @return int the index of the added element (first call to this function for type RTD will return 0, second call 1, ...).
     *         return -1 in case of error
     */
    int addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns);

private:

    Controller* _control;
};

#endif