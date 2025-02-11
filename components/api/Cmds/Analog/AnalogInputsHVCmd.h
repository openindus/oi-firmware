/**
 * @file AnalogInputsHVCmd.h
 * @brief Analog Inputs HV commands
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "AnalogInputsHV.h"
#include "Controller.h"
#include "MasterController.h"

class AnalogInputsHVCmd
{
public:
    AnalogInputsHVCmd(Controller *control) : _control(control) {}

    /**
     * @brief Read the value of AIN.
     * The function return an integer that correspond to the internal voltage of the Analog Input in
     * mV. This value is not calibrated and not converted to the real Analog Input voltage. It is
     * strongly recommended to use analogReadVolts or analogReadMillivolts instead.
     *
     * @param ain ANA input number.
     * @return Value of the AIN input.
     */
    float analogRead(AnalogInput_Num_t num);

    template <typename T>
    inline float analogRead(T num) {
        return analogRead((AnalogInput_Num_t)num);
    }

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
    Controller *_control;
};

#endif