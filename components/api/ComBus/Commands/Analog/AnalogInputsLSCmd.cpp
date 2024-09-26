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
    const uint8_t *ptr = reinterpret_cast<const uint8_t*>(&aIns);
    msgBytes.insert(msgBytes.end(), ptr, ptr + aIns.size()*sizeof(AIn_Num_t));
    
    /* Create new instance of sensor and return sensor index */
    if (_control->request(msgBytes) == 0) {
        index = static_cast<int>(msgBytes[1]);
    }
    
    if (index >= 0)
    {
        switch(type) 
        {
        case RAW_SENSOR:
            if (aIns.size() == 2) {
                raw.emplace_back(_control);
                return index;
            } else {
                ESP_LOGE(TAG, "Raw Sensor requires 2 AINs.");
                return -1;
            }
            break;

        case RTD_PT100:
        case RTD_PT1000:
            if (aIns.size() == 2 || aIns.size() == 3) {
                rtd.emplace_back(_control);
                return index;
            } else {
                ESP_LOGE(TAG, "RTD type requires 2 or 3 AINs.");
                return -1;
            }
            break;

        case THERMOCOUPLE_B:
        case THERMOCOUPLE_E:
        case THERMOCOUPLE_J:
        case THERMOCOUPLE_K:
        case THERMOCOUPLE_N:
        case THERMOCOUPLE_R:
        case THERMOCOUPLE_S:
        case THERMOCOUPLE_T:
            if (aIns.size() == 2) {
                tc.emplace_back(_control);
                return index;
            } else {
                ESP_LOGE(TAG, "THERMOCOUPLE requires 2 AINs.");
                return -1;
            }
            break;

        case STRAIN_GAUGE:
            if (aIns.size() == 4) {
                sg.emplace_back(_control);
                return index;
            } else {
                ESP_LOGE(TAG, "STRAIN_GAUGE requires 4 AINs.");
                return -1;
            }
            break;

        default:
            ESP_LOGE(TAG, "Unknown sensor type.");
            return -1;
        }
    }
}

#endif