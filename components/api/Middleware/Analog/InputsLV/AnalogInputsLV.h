/**
 * @file AnalogInputsLV.h
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "ads866x.h"
#include "AnalogInputs.h"

#define AIN_CURRENT_MODE_RES_VALUE  100.0f
#define AIN_DEFAULT_MODE            AIN_MODE_VOLTAGE
#define AIN_DEFAULT_RANGE           AIN_VOLTAGE_RANGE_0_10V24

class AnalogInputAds866x
{
public:

    AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio);
    int init(AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode);
    int read(void);
    float read(AnalogInput_Unit_t unit);
    void setMode(AnalogInput_Mode_t mode);
    uint8_t getMode(void);
    void setVoltageRange(AnalogInput_VoltageRange_t range);
    uint8_t getVoltageRange(void);
    gpio_num_t getModePin();

private:

    int _num;
    gpio_num_t _modePin;
    AnalogInput_Mode_t _mode;
    AnalogInput_VoltageRange_t _voltage_range;
};


class AnalogInputsLV
{
public:

    static int init(ads866x_config_t *ads866xConfig, const gpio_num_t* cmdGpio, uint8_t nb);

    /**
     * @brief Read a voltage measure on analog pin and return the raw value.
     * 
     * @param num Analog input
     * @return Adc raw value
     */
    static int analogRead(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the AnalogInput.
     *
     * @param num Analog input
     * @return Value of the AIN input
     */
    static float analogReadVolt(AnalogInput_Num_t num);
    
    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param num Analog input
     * @return Measure in mV
     */
    static float analogReadMilliVolt(AnalogInput_Num_t num);

    /**
     * @brief Read a current measure on analog pin
     * 
     * @param num : Analog input
     * @return Measure in A
     */
    static float analogReadAmp(AnalogInput_Num_t num) ;

    /**
     * @brief Read a current measure on analog pin
     * 
     * @param num : Analog input
     * @return Measure in mA
     */
    static float analogReadMilliAmp(AnalogInput_Num_t num);

    /**
     * @brief Set Adc Mode of the given input
     * 
     * @param num Analog input
     * @param mode Voltage or current measurement
     */
    static void analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode);

    /**
     * @brief Get Adc Mode of the given input
     * 
     * @param num Analog input
     * @return Mode (0: voltage, 1: current)
    **/
    static uint8_t analogInputGetMode(AnalogInput_Num_t num);

    /**
     * @brief Set the voltage range of the given input
     * 
     * @param num Analog input
     * @param range Voltage range (5: 0-10.24V, 6: 0-5.12V, 7: 0-2.56V or 8: 0-1.28V)
     */
    static void analogInputVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range);

    /**
     * @brief Get the Voltage range of the given input
     * 
     * @param num Analog input
     * @return Voltage range (5: 0-10.24V, 6: 0-5.12V, 7: 0-2.56V or 8: 0-1.28V)
    **/
    static uint8_t analogInputGetVoltageRange(AnalogInput_Num_t num);

private:

    static uint8_t _nb;
    static AnalogInputAds866x** _ains;

    static float read(AnalogInput_Num_t num, AnalogInput_Unit_t unit);
};