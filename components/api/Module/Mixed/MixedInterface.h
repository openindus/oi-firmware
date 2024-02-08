/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedInterface.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"

class MixedInterface
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
     * @param dinNum DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    virtual int digitalRead(DigitalInputNum_t dinNum) = 0;

    /**
     * @brief Attach a user callback to the DIN interrupts. 
     * First argument is the DIN to attach interrupt, the second argument is the interrupt handler, 
     * third argument is the interrupt mode.
     * 
     * @param din DIN to attach interrupt.
     * @param handler Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    virtual void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode=RISING_MODE, void* arg=NULL) = 0;

    /**
     * @brief Detach interrupt of a given DIN. 
     * First argument is the DIN to detach interrupt, the second argument is the interrupt mode, 
     * third argument is the id of the module that attached the interrupt.
     * 
     * @param din DIN to detach interrupt.
     * @param mode Mode of the interrupt to detach (RISING, FALLING or CHANGE).
     */
    virtual void detachInterrupt(DigitalInputNum_t din) = 0;

    /**
     * @brief Read a voltage measure on analog pins
     * 
     * @param[in] ain : Analog input
     * @return int : Adc raw value
     */
    virtual int analogRead(AnalogInputNum_t ain) = 0;

    /**
     * @brief Read a voltage measure on analog pins
     * 
     * @param[in] ain : Analog input
     * @return int : Measure in specified units
     */
    virtual int analogReadMilliVolts(AnalogInputNum_t ain) = 0;

    /**
     * @brief Set AdcMode of the current input
     * 
     * @param[in] ain : Analog input
     * @param[in] mode : Voltage or current measurement
     */    
    virtual void analogReadMode(AnalogInputNum_t ain, AdcMode_t mode) = 0;

    /**
     * @brief Set adc resolution measurement
     * 
     * @param[in] res : Adc resolution
     */  
    virtual void analogReadResolution(AdcResBits_t res) = 0;

    /**
     * @brief Set adc internal reference voltage
     * 
     * @param[in] ref reference value
     */ 
    virtual void analogReadReference(float ref) = 0;

    /**
     * @brief Set a voltage analog output to desired value
     * 
     * @param[in] aout : Analog output
     * @param[in] value : Desired value
     */ 
    virtual void analogWriteVoltage(AnalogOutputNum_t aout, uint32_t value) = 0;

    /**
     * @brief Set a voltage analog output to desired value
     * 
     * @param aout Analog output
     * @param value Desired value
     */
    virtual void analogWriteVoltageMilliVolts(AnalogOutputNum_t aout, uint32_t value) = 0;

    /**
     * @brief Configure voltage range meaurement of the analog output
     * 
     * @param[in] aout : Analog output
     * @param[in] mode : Configure voltage range output
     */ 
    virtual void analogWriteVoltageMode(AnalogOutputNum_t aout, DacVoltageMode_t mode) = 0;

    /**
     * @brief Set a current analog output to desired value
     * 
     * @param[in] aout : Analog output
     * @param[in] value : Desired value
     */ 
    virtual void analogWriteCurrent(AnalogOutputNum_t aout, uint32_t value) = 0;

    /**
     * @brief Set a current analog output to desired value
     * 
     * @param[in] aout : Analog output
     * @param[in] value : Desired value
     */ 
    virtual void analogWriteCurrentMilliAmps(AnalogOutputNum_t aout, uint32_t value) = 0;

    /**
     * @brief Configure current range meaurement of the analog output
     * 
     * @param[in] aout : Analog output
     * @param[in] mode : Configure current range output
     */ 
    virtual void analogWriteCurrentMode(AnalogOutputNum_t aout, DacCurrentMode_t mode) = 0;

};
