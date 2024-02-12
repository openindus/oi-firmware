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

#if defined(CONFIG_CORE)

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
    static void digitalWrite(DigitalOutputNum_t dout, uint8_t level);

    /**
     * @brief Read an input current level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param din DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    static int digitalRead(DigitalInputNum_t din);

    /**
     * @brief Read the value of AIN.
     * The function return the raw value (0-4096) that correspond to the voltage of the ANA.
     *
     * @param eana ANA input to monitor.
     * @return int Value of the AIN input.
     */
    static int analogRead(AnalogInputNum_t eana);

    /**
     * @brief Read the value of AIN.
     * The function return a float that correspond to the voltage of the ANA (from 0 to 2.6V).
     *
     * @param eana ANA input to monitor.
     * @return float Value of the AIN input.
     */
    static float analogReadMilliVolts(AnalogInputNum_t eana);

    /**
     * @brief Attach a user callback to the DIN interrupts.
     * 
     * @param din DIN to attach interrupt.
     * @param callback function to attach
     * @param args function arguments
     * @param mode mode of interruption on rising edge, 
     * falling edge or both (rising edge by default).
     */
    static void attachInterrupt(DigitalInputNum_t din, void (*callback)(void *), void* args=NULL, InterruptMode_t mode=RISING_MODE);
    
    /**
     * @brief Detach an interrupt to a given DIN.
     * 
     * @param din DIN to detach interrupt.
     */
    static void detachInterrupt(DigitalInputNum_t din, InterruptMode_t mode);

    /**
     * @brief Read the overcurrent status of a given DOUT. 
     * 
     * @param dout DOUT channel to read overcurrent status.
     * @return 0 is current is under 4Amps and 1 is current is above 4Amps
     */
    static uint8_t digitalReadOverCurrent(DigitalOutputNum_t dout);

// protected:

    static void init(void);

private:

    static ioex_device_t *_ioex;

    static esp_adc_cal_characteristics_t _adc1Characteristics;
    static std::map<DigitalInputNum_t, InterruptMode_t> _dinCurrentMode;

    static const ioex_num_t _dout[4];
    static const ioex_num_t _doutSensor[4];
    static const ioex_num_t _din[4];
    static const adc1_channel_t _eana[2];

    static void _controlTask(void *pvParameters);

};

#endif