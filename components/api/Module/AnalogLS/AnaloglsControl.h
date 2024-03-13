/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteControl.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "ModuleControl.h"
#include "ModuleMaster.h"
#include "AnaloglsType.h"

#if !defined(OI_ANALOG_LS)

class AnaloglsControl : public ModuleControl
{
public : 

    AnaloglsControl(uint16_t id = 0) : ModuleControl(id) {}

   float GetChannelData(analog_channel_t channel, rtd_type_t rtd);
   void AddThermocouple(analog_channel_t channel, ads1x4s08_voltage_range_t range, float coef);
   void AddLevelDetector(analog_channel_t channel);
   void AddRTD(analog_channel_t channel, rtd_type_t type, wire_t nb_wire, ads1x4s08_voltage_range_t range, ads1x4s08_idac_mag_t current, float coef);
private :
};

#endif