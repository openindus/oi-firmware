/**
 * @file AnalogInputsLVCmd.h
 * @brief Analog Inputs LV commands
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "ControllerTypes.h"
#include "AnalogInputsLV.h"
#include "Slave.h"
#include "Master.h"

class AnalogInputsLVCmd
{
public:
    AnalogInputsLVCmd(Controller* control) : _controller(control) {}

    /**
     * @brief Read a voltage measure on analog pin and return the raw value.
     *
     * @param num Analog input
     * @return Adc raw value
     */
    int analogRead(AnalogInput_Num_t num);

    template <typename T>
    inline int analogRead(T num) {
        return analogRead((AnalogInput_Num_t)num);
    }

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the AnalogInput.
     *
     * @param num Analog input
     * @return Value of the AIN input
     */
    float analogReadVolt(AnalogInput_Num_t num);

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 10000mV).
     *
     * @param num Analog input
     * @return Measure in mV
     */
    float analogReadMilliVolt(AnalogInput_Num_t num);

    /**
     * @brief Read a current measure on analog pin
     *
     * @param num Analog input
     * @return Measure in A
     */
    float analogReadAmp(AnalogInput_Num_t num);

    /**
     * @brief Read a current measure on analog pin
     *
     * @param num Analog input
     * @return Measure in mA
     */
    float analogReadMilliAmp(AnalogInput_Num_t num);

    /**
     * @brief Set Adc Mode of the given input
     *
     * @param num Analog input
     * @param mode Voltage or current measurement
     */
    void analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode);

    /**
     * @brief Get Adc Mode of the given input
     *
     * @param num Analog input
     * @return Mode (0: voltage, 1: current)
     **/
    uint8_t analogInputGetMode(AnalogInput_Num_t num);

    /**
     * @brief Set the voltage range of the given input
     *
     * @param num Analog input
     * @param range Voltage range (5: 0-10.24V, 6: 0-5.12V, 7: 0-2.56V or 8: 0-1.28V)
     */
    void analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range);

    /**
     * @brief Get the Voltage range of the given input
     *
     * @param num Analog input
     * @return Voltage range (5: 0-10.24V, 6: 0-5.12V, 7: 0-2.56V or 8: 0-1.28V)
     **/
    uint8_t analogInputGetVoltageRange(AnalogInput_Num_t num);

private:
    Controller* _controller;
};

#endif