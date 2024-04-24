/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CoreStandalone.h
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "ModuleStandalone.h"

#if defined(OI_CORE)

class CoreStandalone : public ModuleStandalone
{
public:

    /**
     * @brief Set an output at high or low level.
     * First argument is the DOUT to drive and second argument is the level to set.
     * 
     * @param dout DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    static void digitalWrite(DigitalOutputNum_t doutNum, uint8_t level);

    /**
     * @brief Toggle an output.
     * First argument is the DOUT to drive.
     * 
     * @param dout DOUT to drive.
     */
    static void digitalToggle(DigitalOutputNum_t doutNum);

    /**
     * @brief Read an input current level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param din DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    static int digitalRead(DigitalInputNum_t dinNum);

    /**
     * @brief Read the value of AIN.
     * The function return the raw value (0-4096) that correspond to the voltage of the ANA.
     *
     * @param ain ANA input to monitor.
     * @return int Value of the AIN input.
     */
    static int analogRead(AnalogInput_Num_t ain);

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 2.6V).
     *
     * @param ain ANA input to monitor.
     * @return float Value of the AIN input.
     */
    static int analogReadMilliVolts(AnalogInput_Num_t ain);

    /**
     * @brief Attach a user callback to the DIN interrupts.
     * 
     * @param din DIN to attach interrupt.
     * @param callback function to attach
     * @param mode mode of interruption, 
     * @param args function arguments
     * falling edge or both (rising edge by default).
     */
    void attachInterrupt(DigitalInputNum_t dinNum, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL);
    
    /**
     * @brief Detach an interrupt to a given DIN.
     * 
     * @param din DIN to detach interrupt.
     */
    static void detachInterrupt(DigitalInputNum_t dinNum);

    /**
     * @brief Read the overcurrent status of a given DOUT. 
     * 
     * @param dout DOUT channel to read overcurrent status.
     * @return 0 is current is under 4Amps and 1 is current is above 4Amps
     */
    static uint8_t getCurrentLevel(DigitalOutputNum_t doutNum);

    static void init(void);

    static OI::CAN can;
    static OI::RS rs;

private:

    static ioex_device_t *_ioex;
    static esp_adc_cal_characteristics_t _adc1Characteristics;

    static DigitalOutputs* dout;
    static DigitalInputs* din;

    static void _controlTask(void *pvParameters);

};

#endif