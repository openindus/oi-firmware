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
        for (auto it = data.begin() + 2; it != data.end(); it++) {
            printf("%i\n", *it);
            aIns.push_back((AIn_Num_t)*it);
        }
        int index = AnalogInputsLS::addSensor((Sensor_Type_e)data[1], aIns);
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

    ControllerSlave::addCtrlCallback(REQUEST_RAW_SENSOR_SET_GAIN, [](std::vector<uint8_t>& data) {
        AnalogInputsLS::raw[data[1]].setGain((Sensor_Gain_e)data[2]);
        data.clear();
    });

    ControllerSlave::addCtrlCallback(REQUEST_RAW_SENSOR_READ, [](std::vector<uint8_t>& data) {
        int16_t value = AnalogInputsLS::raw[data[1]].read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(int16_t));
        data[0] = EVENT_RAW_SENSOR_READ;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_RAW_SENSOR_READ_MILLIVOLT, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::raw[data[1]].readMillivolts();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_RAW_SENSOR_READ_MILLIVOLT;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_RTD_READ_RESISTOR, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::rtd[data[1]].readResistor();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_RTD_READ_RESISTOR;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_RTD_READ_TEMPERATURE, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::rtd[data[1]].readTemperature();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_RTD_READ_TEMPERATURE;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_TC_READ_MILLIVOLTS, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::tc[data[1]].readMillivolts();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_TC_READ_MILLIVOLTS;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_TC_READ_TEMPERATURE, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::tc[data[1]].readTemperature();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_TC_READ_TEMPERATURE;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SG_READ, [](std::vector<uint8_t>& data) {
        float value = AnalogInputsLS::sg[data[1]].read();
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
        data.insert(data.end(), ptr, ptr + sizeof(float));
        data[0] = EVENT_SG_READ;
        // Send response on CAN Bus because no return is needed for this message
        ControllerSlave::sendEvent(data);
    });

    ControllerSlave::addCtrlCallback(REQUEST_SG_SET_EXCITATION_MODE, [](std::vector<uint8_t>& data) {
        AnalogInputsLS::sg[data[1]].setExcitationMode((StrainGauge_Excitation_e)data[2]);
        data.clear();
    });

    return 0;
}

#endif