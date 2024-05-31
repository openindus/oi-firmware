/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ControlCmd_AOut.h
 * @author CBA
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_MASTER)

#include "ControlCmd.h"
#include "AnalogOutputs.h"

class ControlCmd_AOut
{
public:

    ControlCmd_AOut(ControlCmd* control) :_control(control) {}

    /**
     * @brief Set the mode of the specified Analog Output
     * 
     * @param num Analog Output number
     * @param mode Analog Output mode
     * @return 0 if success, -1 if error 
    **/
    int analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode);
   
    /**
     * @brief Set the specified Analog Output to desired voltage value
     * 
     * @param num Analog Output number
     * @param value Desired voltage
     * @return 0 if success, -1 if error
     */
    int analogWrite(AnalogOutput_Num_t num, float value);

private:

    ControlCmd* _control;
};

#endif