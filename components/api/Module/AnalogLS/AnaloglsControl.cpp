/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.cpp
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnaloglsControl.h"

#if !defined(CONFIG_ANALOG_LS)

float AnaloglsControl::GetChannelData(analog_channel_t channel, rtd_type_t rtd)
{
    RequestMsg_t msg;
    uint32_t data;
    float temp;
    msg.request = ANALOG_GET_CHANNEL_DATA;
    msg.param = (uint16_t)((channel << 8) | rtd);
    data = request(msg);
    memcpy(&temp, &data, sizeof(float));
    return temp;
}

void AnaloglsControl::AddThermocouple(analog_channel_t channel, ads1x4s08_voltage_range_t range, float coef)
{
    RequestMsg_t msg;
    msg.request = ANALOG_ADD_TC;
    msg.param = (uint16_t)((range << 8) | channel);
    memcpy(&msg.data, &coef, sizeof(uint32_t));
    request(msg);
}

void AnaloglsControl::AddLevelDetector(analog_channel_t channel)
{
    RequestMsg_t msg;
    msg.request = ANALOG_ADD_LEVEL_DETECTOR;
    msg.param = (uint8_t)(channel);
    request(msg);
}

void AnaloglsControl::AddRTD(analog_channel_t channel, rtd_type_t type, wire_t nb_wire, ads1x4s08_voltage_range_t range, ads1x4s08_idac_mag_t current, float coef)
{
    RequestMsg_t msg;
    msg.request = ANALOG_ADD_RTD;
    msg.param = (uint32_t)((channel<<16)|(type<<12)|(nb_wire<<8)|(range << 4) | current);
    memcpy(&msg.data, &coef, sizeof(uint32_t));
    request(msg);
}


#endif