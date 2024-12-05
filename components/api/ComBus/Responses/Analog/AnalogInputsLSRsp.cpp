/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLSRsp.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLSRsp.h"

#if defined(MODULE_SLAVE)

int AnalogInputsLSRsp::init(void)
{
    
    ControllerSlave::addCtrlCallback(REQUEST_ADD_SENSOR, [](std::vector<uint8_t>& data) {
        std::vector<AIn_Num_t> aIns;

        std::array<AIn_Num_e, 4> ainPins = {AIN_NULL, AIN_NULL, AIN_NULL, AIN_NULL};

        for (auto it = data.begin() + 2; it != data.end(); it++) {
            aIns.push_back((AIn_Num_t)*it);
        }
        ainPins[0] = aIns[0];
        ainPins[1] = aIns[1];
        if (aIns.size() >= 3) {
            ainPins[2] = aIns[2];
        }
        if (aIns.size() >= 4) {
            ainPins[3] = aIns[3];
        }
        int index = AnalogInputsLS::addSensor((Sensor_Type_e)data[1], ainPins);
        data.clear();
        data.push_back(REQUEST_ADD_SENSOR);
        data.push_back((uint8_t)index);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SET_STABILIZATION_TIME, [](std::vector<uint8_t>& data) {
        int* value = reinterpret_cast<int*>(&data[1]);
        AnalogInputsLS::setStabilizationTime(*value);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_SET_ACQUISITION_TIME, [](std::vector<uint8_t>& data) {
        AnalogInputsLS::setAcquisitionTime((AcquisitionDuration_e)data[1]);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_SET_PARAMETER, [](std::vector<uint8_t>& data) {
        Sensor_Parameter_e parameter = (Sensor_Parameter_e) data[2];
        Sensor_Parameter_Value_u value = {.value = (int8_t) data[3]};

        AnalogInputsLS::sensors[data[1]]->setParameter(parameter, value);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_READ, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_READ;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->readMillivolts();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_READ_MILLIVOLT;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_READ_RESISTANCE, [](std::vector<uint8_t>& data) {
        printf("read resistor of %i\n", data[1]);
        float value = AnalogInputsLS::sensors[data[1]]->readResistor();
        printf("val:%f\n", value);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_READ_RESISTANCE;
        // Send response on CAN Bus because no return is needed for this message
        printf("Send response\n");
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_READ_TEMPERATURE, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->readTemperature();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_READ_TEMPERATURE;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SENSOR_READ_RAW, [](std::vector<uint8_t>& data) {
        int16_t value = AnalogInputsLS::sensors[data[1]]->raw_read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int16_t));
        data[0] = EVENT_SENSOR_READ_RAW;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    return 0;
}

#endif