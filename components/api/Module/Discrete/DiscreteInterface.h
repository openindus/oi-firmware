/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscreteInterface.h
 * @brief Functions for Discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class DiscreteInterface
{
public:

    /**
     * @brief Set an output at high or low level.
     * First argument is the DOUT to drive and second argument is the level to set.
     * 
     * @param dout DOUT to drive.
     * @param level DOUT level, HIGH or LOW.
     */
    virtual void digitalWrite(DigitalOutputNum_t dout, uint8_t level) = 0;

    /**
     * @brief Read an input current level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param din DIN to monitor.
     * @return Value of the DIN input (1 or 0).
     */
    virtual int digitalRead(DigitalInputNum_t din) = 0;

    /**
     * @brief Attach a user callback to the DIN interrupts. 
     * First argument is the DIN to attach interrupt, the second argument is the interrupt handler, third argument is the interrupt mode.
     * 
     * @param din DIN to attach interrupt.
     * @param function function to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    virtual void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL) = 0;

    /**
     * @brief Detach interrupt of a given DIN. 
     * First argument is the DIN to detach interrupt, the second argument is the interrupt mode, third argument is the id of the module that attached the interrupt.
     * 
     * @param din DIN to detach interrupt.
     * @param mode Mode of the interrupt to detach (RISING, FALLING or CHANGE).
     */
    virtual void detachInterrupt(DigitalInputNum_t din) = 0;

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the raw value of the ANA in millivolts (from 0 to 3100mV).
     * The returned value correspond to the raw internal voltage and not the output voltage. 
     * It is better to use analogReadVolts or analogReadMillivolts.
     * @param ain ANA input.
     * @return int Value of the AIN input.
     */
    virtual int analogRead(AnalogInput_Num_t ainNum) = 0;

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30V).
     *
     * @param ain ANA input.
     * @return float Value of the AIN input.
     */
    virtual float analogReadVolt(AnalogInput_Num_t ainNum) = 0;

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 30000mV).
     *
     * @param ain ANA input.
     * @return float Value of the AIN input.
     */
    virtual float analogReadMilliVolt(AnalogInput_Num_t ainNum) = 0;

};