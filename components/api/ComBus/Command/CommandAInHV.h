/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CommandAInHV.h
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

#include "Controller.h"
#include "AnalogInputsHV.h"

class CommandAInHV
{
public:

    CommandAInHV(Controller* control) :_control(control) {}

    /**
     * @brief Read the value of AIN.
     * The function return an integer that correspond to the internal voltage of the Analog Input in mV.
     * This value is not calibrated and not converted to the real Analog Input voltage.
     * It is strongly recommended to use analogReadVolts or analogReadMillivolts instead.
     * 
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    float analogRead(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30V).
     *
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    float analogReadVolt(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    float analogReadMilliVolt(AnalogInput_Num_t num);
    
private:

    Controller* _control;
};

#endif