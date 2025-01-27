/**
 * @file  AnalogOutputsCmd.h
 * @brief Analog Outputs commands
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "AnalogOutputs.h"
#include "Controller.h"
#include "ControllerMaster.h"

class AnalogOutputsCmd
{
public:
    AnalogOutputsCmd(Controller *control) : _control(control) {}

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

    template <typename T1, typename T2>
    inline int analogWrite(T1 num, T2 value) {
        return analogWrite((AnalogOutput_Num_t)num, (float)value);
    }

private:
    Controller *_control;
};

#endif