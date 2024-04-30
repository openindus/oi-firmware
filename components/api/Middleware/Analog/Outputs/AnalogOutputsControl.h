/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogOutputsControl.h
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

#include "ModuleControl.h"
#include "AnalogOutputs.h"

class AnalogOutputsControl
{
public:

    AnalogOutputsControl(ModuleControl* control) :_control(control) {}

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

    ModuleControl* _control;
};

#endif