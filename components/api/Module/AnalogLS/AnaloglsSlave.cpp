/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteSlave.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnaloglsSlave.h"

#if defined (OI_ANALOG_LS)


void AnaloglsSlave::init(void)
{
    AnaloglsStandalone::init();
    ModuleSlave::init();

    onRequest(ANALOG_ADD_VOLTMETER,[](Module_RequestMsg_t msg) -> uint32_t {
        analog_channel_t channel = (analog_channel_t)msg.param;
        ads1x4s08_voltage_range_t range = (ads1x4s08_voltage_range_t)msg.data;
        AnaloglsStandalone::AddVoltmeter(channel, range);
        return 0;
    });

    onRequest(ANALOG_GET_CHANNEL_DATA,[](Module_RequestMsg_t msg) -> uint32_t {
        rtd_type_t rtd = (rtd_type_t)(msg.param & 0x0000FF);
        analog_channel_t channel = (analog_channel_t)((msg.param & 0x00FF00) >> 8);
        uint32_t data = 0;
        float float_data = AnaloglsStandalone::GetChannelData(channel,rtd);
        memcpy(&data, &float_data, sizeof(uint32_t));
        return data;
    }); 

    onRequest(ANALOG_ADD_TC,[](Module_RequestMsg_t msg) -> uint32_t {
        analog_channel_t channel = (analog_channel_t)(msg.param & 0x0000FF);
        ads1x4s08_voltage_range_t range = (ads1x4s08_voltage_range_t)((msg.param & 0x00FF00) >> 8);
        float coef = 0;
        uint32_t data = msg.data; 
        memcpy(&coef, &data, sizeof(float));
        AnaloglsStandalone::AddThermocouple(channel,range,coef);
        return 0;
    });

    onRequest(ANALOG_ADD_LEVEL_DETECTOR,[](Module_RequestMsg_t msg) -> uint32_t {
        analog_channel_t channel = (analog_channel_t)msg.param;
        AnaloglsStandalone::AddLevelDetector(channel);
        return 0;
    });

    onRequest(ANALOG_ADD_RTD,[](Module_RequestMsg_t msg) -> uint32_t {
        analog_channel_t channel = (analog_channel_t)((msg.param & 0xFF0000)>>16);
        rtd_type_t type = (rtd_type_t)((msg.param & 0x00F000)>>12);
        wire_t nb_wire = (wire_t)((msg.param & 0x000F00)>>8);
        ads1x4s08_voltage_range_t range = (ads1x4s08_voltage_range_t)((msg.param & 0x0000F0)>>4);
        ads1x4s08_idac_mag_t current = (ads1x4s08_idac_mag_t)(msg.param & 0x00000F);
        float coef = 0;
        uint32_t data = msg.data; 
        memcpy(&coef, &data, sizeof(float));
        AnaloglsStandalone::AddRTD(channel, type, nb_wire, range, current, coef);
        return 0;
    });

}

#endif
