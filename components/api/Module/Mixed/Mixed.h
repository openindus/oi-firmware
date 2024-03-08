/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Mixed.h
 * @brief Functions for Mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "AnalogOutput.h"

class Mixed
{
public:

    /**
     * @brief Read an input current level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    virtual int digitalRead(DigitalInputNum_t num) = 0;

    /**
     * @brief Attach a user callback to the DIN interrupts. 
     * First argument is the DIN to attach interrupt, the second argument is the interrupt handler, 
     * third argument is the interrupt mode.
     * 
     * @param num DIN to attach interrupt.
     * @param handler Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    virtual void attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, 
        InterruptMode_t mode=RISING_MODE, void* arg=NULL) = 0;

    /**
     * @brief Detach interrupt of a given DIN. 
     * First argument is the DIN to detach interrupt, the second argument is the interrupt mode, 
     * third argument is the id of the module that attached the interrupt.
     * 
     * @param num DIN to detach interrupt.
     * @param mode Mode of the interrupt to detach (RISING, FALLING or CHANGE).
     */
    virtual void detachInterrupt(DigitalInputNum_t num) = 0;

    /**
     * @brief Set an output at high or low level.
     * First argument is the DOUT to drive and second argument is the level to set.
     * 
     * @param num DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    virtual void digitalWrite(DigitalOutputNum_t num, uint8_t level) = 0;

    /**
     * @brief Set AdcMode of the current input
     * 
     * @param[in] num : Analog input
     * @param[in] mode : Voltage or current measurement
     */    
    virtual void analogInputMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode) = 0;

    /**
     * @brief Set adc resolution measurement
     * 
     * @param[in] res : Adc resolution
     */  
    virtual void analogInputResolution(AnalogInput_Resolution_t res) = 0;

    /**
     * @brief Set adc internal reference voltage
     * 
     * @param[in] ref reference value
     */ 
    virtual void analogInputReference(float ref) = 0;

    /**
     * @brief Read a voltage measure on analog pins
     * 
     * @param[in] num : Analog input
     * @return int : Adc raw value
     */
    virtual int analogRead(AnalogInput_Num_t num) = 0;

    /**
     * @brief Read a voltage measure on analog pins
     * 
     * @param[in] num : Analog input
     * @return float : Measure in specified units
     */
    virtual float analogReadMilliVolts(AnalogInput_Num_t num) = 0;

    /**
     * @brief Configure analog output mode
     * 
     * @param num Analog output num
     * @param mode mode: 
     * MODE_M10V5_10V5
     * MODE_0mA_20mA
     */
    virtual void analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode) = 0;

    /**
     * @brief Set a voltage analog output to desired value
     * 
     * @param num Analog output num
     * @param value depends on analog output mode
     */
    virtual void analogWrite(AnalogOutput_Num_t num, float value) = 0;

};