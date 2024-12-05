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

#if !defined(MODULE_STANDALONE) && !defined(MODULE_SLAVE)

static const char TAG[] = "AnalogInputsLSCmd";

GenericSensorCmd::GenericSensorCmd(Controller* control, uint8_t index) : _control(control), _index(index)
{
    /* Create the queue for wait function and add callback for CAN event */
    _readEvent = xQueueCreate(1, sizeof(uint8_t*));
}

int16_t GenericSensorCmd::getInt16(Protocol_Event_e event, Protocol_Request_e request)
{
    // Add callback (remove callback if it already exists)
    ControllerMaster::removeEventCallback(event, _control->getId());
    ControllerMaster::addEventCallback(event, _control->getId(), [this](uint8_t* data) {
        xQueueSend(_readEvent, &data, pdMS_TO_TICKS(100));
    });

    // Send a message to slave to request a read but do not wait a response
    std::vector<uint8_t> msgBytes = {(uint8_t)request, _index};
    _control->request(msgBytes, false);

    // Wait for event
    uint8_t* data = NULL;
    xQueueReset(_readEvent);
    xQueueReceive(_readEvent, &data, portMAX_DELAY);
    int16_t* ret = reinterpret_cast<int16_t*>(&data[2]);
    return *ret;
}

float GenericSensorCmd::getFloat(Protocol_Event_e event, Protocol_Request_e request)
{
    // Add callback (remove callback if it already exists)
    ControllerMaster::removeEventCallback(event, _control->getId());
    ControllerMaster::addEventCallback(event, _control->getId(), [this](uint8_t* data) {
        xQueueSend(_readEvent, &data, pdMS_TO_TICKS(100));
    });

    // Send a message to slave to request a read but do not wait a response
    std::vector<uint8_t> msgBytes = {(uint8_t)request, _index};
    _control->request(msgBytes, false);

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

void RawSensorCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SENSOR_SET_PARAMETER, _index, (uint8_t) parameter, (uint8_t) value.value};
    _control->request(msgBytes);
}

float RawSensorCmd::readMillivolts(void)
{
    return getFloat(EVENT_SENSOR_READ_MILLIVOLT, REQUEST_SENSOR_READ_MILLIVOLT);
}

float RTDCmd::readResistor(void)
{
    return getFloat(EVENT_SENSOR_READ_RESISTANCE, REQUEST_SENSOR_READ_RESISTANCE);
}

float RTDCmd::readTemperature(void)
{
    return getFloat(EVENT_SENSOR_READ_TEMPERATURE, REQUEST_SENSOR_READ_TEMPERATURE);
}

float ThermocoupleCmd::readMillivolts(void)
{
    return getFloat(EVENT_SENSOR_READ_MILLIVOLT, REQUEST_SENSOR_READ_MILLIVOLT);
}

float ThermocoupleCmd::readTemperature(void)
{
    return getFloat(EVENT_SENSOR_READ_TEMPERATURE, REQUEST_SENSOR_READ_TEMPERATURE);
}

void StrainGaugeCmd::setSGExcitationMode(StrainGauge_Excitation_e excitation)
{
    setParameter(PARAMETER_SG_EXCITATION, (union Sensor_Parameter_Value_u) {.sg_excitation=excitation});
}

void StrainGaugeCmd::setParameter(Sensor_Parameter_e parameter, Sensor_Parameter_Value_u value)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SENSOR_SET_PARAMETER, _index, (uint8_t) parameter, (uint8_t) value.value};
    _control->request(msgBytes);
}

void AnalogInputsLSCmd::setAcquisitionTime(AcquisitionDuration_e duration)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_ACQUISITION_TIME, (uint8_t)duration};
    _control->request(msgBytes);
}

void AnalogInputsLSCmd::setStabilizationTime(int duration)
{
    std::vector<uint8_t> msgBytes = {REQUEST_SET_STABILIZATION_TIME};
    uint8_t *ptr = reinterpret_cast<uint8_t*>(&duration);
    msgBytes.insert(msgBytes.end(), ptr, ptr + sizeof(int));
    _control->request(msgBytes);
}

int AnalogInputsLSCmd::addSensor(Sensor_Type_e type, const std::vector<AIn_Num_t>& aIns)
{
    int index = -1;

    /* Send message */
    std::vector<uint8_t> msgBytes = {REQUEST_ADD_SENSOR, (uint8_t)type};
    for (auto it = aIns.begin(); it != aIns.end(); it++) {
        msgBytes.push_back((uint8_t)*it);
    }
    
    /* Create new instance of sensor and return sensor index */
    if (_control->request(msgBytes) == 0) {
        index = static_cast<int>(msgBytes[1]);
    }

    GenericSensorCmd *sensor_ptr = NULL;

    if (index >= 0)
    {
        switch(type) 
        {
        case RAW_SENSOR:
            if (aIns.size() < 2) {
                ESP_LOGE(TAG, "Raw sensor require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new RawSensorCmd(_control, index);
            sensors.emplace_back(sensor_ptr);
            return index;
        case RTD_PT100:
        case RTD_PT1000:
            if (aIns.size() < 2) {
                ESP_LOGE(TAG, "RTD require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new RTDCmd(_control, index);
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
            if (aIns.size() < 2) {
                ESP_LOGE(TAG, "Thermocouple require at least 2 AINS.");
                return -1;
            }
            sensor_ptr = new ThermocoupleCmd(_control, index);
            sensors.emplace_back(sensor_ptr);
            return index;
        case STRAIN_GAUGE:
            if (aIns.size() < 4) {
                ESP_LOGE(TAG, "Strain gauge require 4 AINs.");
                return -1;
            }
            sensor_ptr = new StrainGaugeCmd(_control, index);
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

#endif