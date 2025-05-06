/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLSCmd.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLSCmd.h"

#if defined(MODULE_MASTER)

static const char TAG[] = "AnalogInputsLSCmd";

GenericSensorCmd::GenericSensorCmd(Controller* control, uint8_t index, Sensor_Type_e type, std::array<AIn_Num_t, 4> ain_pins) :
_controller(control),
_index(index),
_type(type),
_ain_pins(ain_pins)
{
    /* Create the queue for wait function and add callback for CAN event */
    _readEvent = xQueueCreate(1, sizeof(uint8_t*));
}

int16_t GenericSensorCmd::getInt16(uint8_t event, uint8_t request)
{
    // Add callback (remove callback if it already exists)
    Master::removeEventCallback(event, _controller->getId());
    Master::addEventCallback(event, _controller->getId(), [this](uint8_t* data) {
        xQueueSend(_readEvent, &data, pdMS_TO_TICKS(100));
    });

    // Send a message to slave to request a read but do not wait a response
    std::vector<uint8_t> msgBytes = {(uint8_t)request, _index};
    _controller->performRequest(msgBytes, false);

    // Wait for event
    uint8_t* data = NULL;
    xQueueReset(_readEvent);
    xQueueReceive(_readEvent, &data, portMAX_DELAY);
    int16_t* ret = reinterpret_cast<int16_t*>(&data[2]);
    return *ret;
}

float GenericSensorCmd::getFloat(uint8_t event, uint8_t request)
{
    // Add callback (remove callback if it already exists)
    Master::removeEventCallback(event, _controller->getId());
    Master::addEventCallback(event, _controller->getId(), [this](uint8_t* data) {
        xQueueSend(_readEvent, &data, pdMS_TO_TICKS(100));
    });

    // Send a message to slave to request a read but do not wait a response
    std::vector<uint8_t> msgBytes = {(uint8_t)request, _index};
    _controller->performRequest(msgBytes, false);

    // Wait for event
    uint8_t* data = NULL;
    xQueueReset(_readEvent);
    xQueueReceive(_readEvent, &data, portMAX_DELAY);
    float* ret = reinterpret_cast<float*>(&data[2]);
    return *ret;
}

int16_t GenericSensorCmd::raw_read(void)
{
    return getInt16(EVENT_SENSOR_READ_RAW, REQUEST_SENSOR_READ_RAW);
}

float GenericSensorCmd::read(void)
{
    return getFloat(EVENT_SENSOR_READ, REQUEST_SENSOR_READ);
}

void GenericSensorCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SENSOR_SET_PARAMETER, _index, *((uint8_t *) &parameter)};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&value.value);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int32_t));
    _controller->performRequest(msgBytes);
}

void GenericSensorCmd::setAcquisitionTime(AcquisitionDuration_e duration)
{
    setParameter(PARAMETER_ACQUISITION_TIME, (union Sensor_Parameter_Value_u) {.acquisition_time=duration});
}

void GenericSensorCmd::setStabilizationTime(int duration)
{
    setParameter(PARAMETER_STABILIZATION_TIME, (union Sensor_Parameter_Value_u) {.stabilization_time=duration});
}

float RawSensorCmd::readMillivolts(void)
{
    return getFloat(EVENT_SENSOR_READ_MILLIVOLT, REQUEST_SENSOR_READ_MILLIVOLT);
}

void RawSensorCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_ACQUISITION_TIME:
        case PARAMETER_STABILIZATION_TIME:
        case PARAMETER_GAIN:
        case PARAMETER_BIAS:
        case PARAMETER_REFERENCE:
        case PARAMETER_EXCITATION_CURRENT:
        case PARAMETER_MUX_HS_INDEX:
        case PARAMETER_MUX_HS_INPUT:
        case PARAMETER_MUX_LS_INDEX:
        case PARAMETER_MUX_LS_OUTPUT:
            GenericSensorCmd::setParameter(parameter, value);
            return;
        default:
            SENSOR_FUNCTIONNALITY_NOT_FOUND
    }
}

float RTDCmd::readResistor(void)
{
    return getFloat(EVENT_SENSOR_READ_RESISTANCE, REQUEST_SENSOR_READ_RESISTANCE);
}

float RTDCmd::readTemperature(void)
{
    return getFloat(EVENT_SENSOR_READ_TEMPERATURE, REQUEST_SENSOR_READ_TEMPERATURE);
}

void RTDCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_ACQUISITION_TIME:
        case PARAMETER_STABILIZATION_TIME:
            GenericSensorCmd::setParameter(parameter, value);
            return;
        default:
            SENSOR_FUNCTIONNALITY_NOT_FOUND
    }
}

float ThermocoupleCmd::readMillivolts(void)
{
    return getFloat(EVENT_SENSOR_READ_MILLIVOLT, REQUEST_SENSOR_READ_MILLIVOLT);
}

float ThermocoupleCmd::readTemperature(void)
{
    return getFloat(EVENT_SENSOR_READ_TEMPERATURE, REQUEST_SENSOR_READ_TEMPERATURE);
}

void ThermocoupleCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_ACQUISITION_TIME:
        case PARAMETER_STABILIZATION_TIME:
            GenericSensorCmd::setParameter(parameter, value);
            return;
        default:
            SENSOR_FUNCTIONNALITY_NOT_FOUND
    }
}

void StrainGaugeCmd::setSGExcitationMode(StrainGauge_Excitation_e excitation)
{
    setParameter(PARAMETER_SG_EXCITATION, (union Sensor_Parameter_Value_u) {.sg_excitation=excitation});
}

void StrainGaugeCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    switch (parameter) {
        case PARAMETER_ACQUISITION_TIME:
        case PARAMETER_STABILIZATION_TIME:
        case PARAMETER_SG_EXCITATION:
            GenericSensorCmd::setParameter(parameter, value);
            return;
        default:
            SENSOR_FUNCTIONNALITY_NOT_FOUND
    }
}

int AnalogInputsLSCmd::addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns)
{
    int index = -1;
    std::array<AIn_Num_t, 4> aInsArray = {AIN_NULL, AIN_NULL, AIN_NULL, AIN_NULL};

    /* Verify aIns validity */
    if (!std::all_of(aIns.begin(), aIns.end(), [](AIn_Num_t aIn) {
        return (aIn >= AIN_A_P && aIn < AIN_MAX) || aIn == AIN_NULL;
    })) {
        ESP_LOGE(TAG, "One or more AINs are out of range (0 to AIN_MAX - 1).");
        return -1;
    }

    uint8_t nbr_pin_set = 0;
    for (int i = 0; i < aIns.size(); i++) {
        aInsArray[i] = aIns[i];
        if (aIns[i] == AIN_NULL) {
            break;
        }
        nbr_pin_set++;
    }

    /* Send message */
    std::vector<uint8_t> msgBytes = {REQUEST_ADD_SENSOR, (uint8_t)type};
    for (auto it = aIns.begin(); it != aIns.end(); it++) {
        msgBytes.push_back((uint8_t)*it);
    }

    /* Create new instance of sensor and return sensor index */
    if (_controller->performRequest(msgBytes) == 0) {
        index = static_cast<int>(msgBytes[1]);
    }

    GenericSensorCmd *sensor_ptr = NULL;

    if (index >= 0)
    {
        switch(type) 
        {
        case RAW_SENSOR:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "Raw sensor require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new RawSensorCmd(_controller, index, type, aInsArray);
            sensors.emplace_back(sensor_ptr);
            return index;
        case RTD_PT100:
        case RTD_PT1000:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "RTD require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new RTDCmd(_controller, index, type, aInsArray);
            sensors.emplace_back(sensor_ptr);
            return index;
        case THERMOCOUPLE_B:
        case THERMOCOUPLE_E:
        case THERMOCOUPLE_J:
        case THERMOCOUPLE_K:
        case THERMOCOUPLE_N:
        case THERMOCOUPLE_R:
        case THERMOCOUPLE_S:
        case THERMOCOUPLE_T:
            if (nbr_pin_set < 2) {
                ESP_LOGE(TAG, "Thermocouple require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new ThermocoupleCmd(_controller, index, type, aInsArray);
            sensors.emplace_back(sensor_ptr);
            return index;
        case STRAIN_GAUGE:
            if (nbr_pin_set < 4) {
                ESP_LOGE(TAG, "Strain gauge require 4 AINs.");
                return -1;
            }
            sensor_ptr = new StrainGaugeCmd(_controller, index, type, aInsArray);
            sensors.emplace_back(sensor_ptr);
            return index;
        default:
            ESP_LOGE(TAG, "Unknown sensor type.");
            return -1;
        }
    }
    ESP_LOGE(TAG, "The index given by the AnalogLS is describing an error.");
    return -1;
}

static void print_sensor_type(enum Sensor_Type_e type)
{
    const char *type_str = NULL;

    switch (type) {
    case RAW_SENSOR:
        type_str = "raw sensor";
        break;
    case RTD_PT100:
        type_str = "RTD PT100";
        break;
    case RTD_PT1000:
        type_str = "RTD PT1000";
        break;
    case THERMOCOUPLE_B:
        type_str = "thermocouple B";
        break;
    case THERMOCOUPLE_E:
        type_str = "thermocouple E";
        break;
    case THERMOCOUPLE_J:
        type_str = "thermocouple J";
        break;
    case THERMOCOUPLE_K:
        type_str = "thermocouple K";
        break;
    case THERMOCOUPLE_N:
        type_str = "thermocouple N";
        break;
    case THERMOCOUPLE_R:
        type_str = "thermocouple R";
        break;
    case THERMOCOUPLE_S:
        type_str = "thermocouple S";
        break;
    case THERMOCOUPLE_T:
        type_str = "thermocouple T";
        break;
    case STRAIN_GAUGE:
        type_str = "strain gauge";
        break;
    default:
        type_str = "unknown";
        break;
    }
    write(1, type_str, strlen(type_str));
}

static void print_sensor(GenericSensorCmd *sensor)
{
    uint32_t index = sensor->get_index();
    enum Sensor_Type_e type = sensor->get_type();
    std::array<AIn_Num_t, 4> ains = sensor->get_ain_pins();
    printf("\t %u - ", index);
    fflush(stdout);
    print_sensor_type(type);
    printf(", ains [");
    for (int i = 0; i < ains.size(); i++) {
        if (ains[i] == -1) {
            break;
        }
        if (i != 0) {
            printf(", ");
        }
        printf("%hhd", ains[i]);
    }
    printf("]\n");
}

int AnalogInputsLSCmd::list_sensors(void)
{
    printf("There are %d sensors : \n", sensors.size());
    for (size_t i = 0; i < sensors.size(); i++) {
        print_sensor(sensors[i]);
    }
    return sensors.size();
}

#endif