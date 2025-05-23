/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogInputsLSCmdHandler.h
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogInputsLSCmdHandler.h"

#if defined(MODULE_SLAVE)

int AnalogInputsLSCmdHandler::init(void)
{
    
    Slave::addCallback(CALLBACK_ADD_SENSOR, [](std::vector<uint8_t>& data) {
        std::vector<AIn_Num_t> aIns;

        for (auto it = data.begin() + 2; it != data.end(); it++) {
            aIns.push_back((AIn_Num_t)(int8_t)*it);
        }

        int index = AnalogInputsLS::addSensor((Sensor_Type_e)data[1], aIns);
        data.clear();
        data.push_back(CALLBACK_ADD_SENSOR);
        data.push_back((uint8_t)index);
    });

    Slave::addCallback(CALLBACK_SENSOR_SET_PARAMETER, [](std::vector<uint8_t>& data) {
        Sensor_Parameter_e parameter = (Sensor_Parameter_e) data[2];
        int32_t* int_value = reinterpret_cast<int32_t *>(&data[3]);
        Sensor_Parameter_Value_u value = {.value = *int_value};

        AnalogInputsLS::sensors[data[1]]->setParameter(parameter, value);
        data.clear();
    });

    Slave::addCallback(CALLBACK_SENSOR_READ, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_VALUE;
        // Send response on CAN Bus because no return is needed for this message
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_SENSOR_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->readMillivolts();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_VALUE_MILLIVOLT;
        // Send response on CAN Bus because no return is needed for this message
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_SENSOR_READ_RESISTANCE, [](std::vector<uint8_t>& data) {
        printf("read resistor of %i\n", data[1]);
        float value = AnalogInputsLS::sensors[data[1]]->readResistor();
        printf("val:%f\n", value);
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_VALUE_RESISTANCE;
        // Send response on CAN Bus because no return is needed for this message
        printf("Send response\n");
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_SENSOR_READ_TEMPERATURE, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sensors[data[1]]->readTemperature();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SENSOR_VALUE_TEMPERATURE;
        // Send response on CAN Bus because no return is needed for this message
        Slave::sendEvent(data);
    });

    Slave::addCallback(CALLBACK_SENSOR_READ_RAW, [](std::vector<uint8_t>& data) {
        int16_t value = AnalogInputsLS::sensors[data[1]]->raw_read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int16_t));
        data[0] = EVENT_SENSOR_VALUE_RAW;
        // Send response on CAN Bus because no return is needed for this message
        Slave::sendEvent(data);
    });

    Slave::addResetCallback([](void) {
        AnalogInputsLS::resetSensors();
    });

    return 0;
}

#endif