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
#include "global_sensor.hpp"

#define SENSOR_TAG "Sensor"
#define SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE "The sensor you requested doesn't have this functionnality (%s)."
#define SENSOR_FUNCTIONNALITY_NOT_FOUND ESP_LOGE(SENSOR_TAG, SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE, __PRETTY_FUNCTION__);
#define SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(value) ESP_LOGE(SENSOR_TAG, SENSOR_FUNCTIONNALITY_NOT_FOUND_MESSAGE, __PRETTY_FUNCTION__); return value;

class GenericSensorCmd
{
public:

    int16_t raw_read(void);

    /**
     * The read function is a general function. Different sensors produce different units.
     * Here are the functions called for each sensors :
     * - RawSensor : raw_read converted to float
     * - RTD : readTemperature method (the RTD specific one)
     * - Termocouple : readTemperature method (the thermocouple specific one)
     * - StrainGauge : specific read method (returns a raw value between -256 and +256)
     */
    float read(void);
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);
    virtual inline float readMillivolts(void) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(NAN) }
    virtual inline float readResistor(void) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(NAN) }
    virtual inline float readTemperature(void) { SENSOR_FUNCTIONNALITY_NOT_FOUND_RETURN(NAN) }
    virtual inline void setSGExcitationMode(StrainGauge_Excitation_e excitation) { SENSOR_FUNCTIONNALITY_NOT_FOUND }

    inline uint8_t get_index(void) { return _index; }
    inline Sensor_Type_e get_type(void) { return _type; }
    inline std::array<AIn_Num_t, 4> get_ain_pins(void) { return _ain_pins; }

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

protected:
    GenericSensorCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins);
    int16_t getInt16(Protocol_Event_e event, Protocol_Request_e request);
    float getFloat(Protocol_Event_e event, Protocol_Request_e request);
    Controller* _control;
    uint8_t _index;
    Sensor_Type_e _type;
    std::array<AIn_Num_t, 4> _ain_pins;
private:
    QueueHandle_t _readEvent;
};

class RawSensorCmd : public GenericSensorCmd
{
public:
    RawSensorCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins) : GenericSensorCmd(control, index, type, ain_pins) {}
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);
    float readMillivolts(void);
};

class RTDCmd : public GenericSensorCmd
{
public:
    RTDCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins) : GenericSensorCmd(control, index, type, ain_pins) {}
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);
    float readResistor(void);
    float readTemperature(void);
};

class ThermocoupleCmd : public GenericSensorCmd
{
public:
    ThermocoupleCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins) : GenericSensorCmd(control, index, type, ain_pins) {}
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);
    float readMillivolts(void);
    float readTemperature(void);
};

class StrainGaugeCmd : public GenericSensorCmd
{
public:
    StrainGaugeCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins) : GenericSensorCmd(control, index, type, ain_pins) {}
    void setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value);
    void setSGExcitationMode(StrainGauge_Excitation_e excitation);
};

class AnalogInputsLSCmd
{
public:
    /* List of sensors */
    std::vector<GenericSensorCmd *> sensors;

    AnalogInputsLSCmd(Controller* control) : _control(control) {}

    /**
     * @brief Add a new sensor
     *
     * @param type [RAW_SENSOR; RTD_TWO_WIRE; RTD_THREE_WIRE; THERMOCOUPLE[B|E|J|K|N|R|S|T]; STRAIN_GAUGE]
     * @param aIns Analog Inputs (vector) (AIN_A_P to AIN_E_N)
     * @return int the index of the added element (first call to this function for type RTD will return 0, second call 1, ...).
     *         return -1 in case of error
     */
    int addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns);

    /**
     * @brief Lists all sensors
     *
     * @return nbr of sensors
     */
    int list_sensors(void);

private:

    Controller* _control;
};

#endif
