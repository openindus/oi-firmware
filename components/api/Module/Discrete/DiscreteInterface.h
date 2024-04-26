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
     * @brief Read a voltage measure
     * @param[in] ain : Analog input
     * @return int : Measure in raw
     */
    virtual int analogRead(AnalogInput_Num_t ain) = 0;

    /**
     * @brief Read a voltage measure
     * 
     * @param ain Analog input
     * @return int Measure in millivolts
     */
    virtual int analogReadMilliVolt(AnalogInput_Num_t ain) = 0;

    /**
     * @brief Set a PWM output at a given duty cycle. 
     * First argument is the DOUT to drive and second argument is the duty cycle to set (from 0 to 255).
     * 
     * @param dout DOUT to drive.
     * @param duty Value of the duty cycle (from 0 to 255).
     */
    virtual void analogWrite(DigitalOutputNum_t dout, uint8_t duty) = 0;

};