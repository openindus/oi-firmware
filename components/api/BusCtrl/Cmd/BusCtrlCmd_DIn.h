/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCtrlCmd_DIn.h
 * @author AFL
 * @version 0.1
 * @date 2024-04-29
 * 
 * For more information on OpenIndus:
 * @see https://openindus.com
 * 
 * @copyright Copyright (c) 2024
**/

#pragma once

#if defined(MODULE_MASTER)

#include "BusCtrlCmd.h"
#include "DigitalInputs.h"

class BusCtrlCmd_DIn
{
public:

    BusCtrlCmd_DIn(BusCtrlCmd* control) : _control(control), _isrCallback{NULL}  {}

    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    int digitalRead(DIn_Num_t num);

    /**
     * @brief Attach a user callback to the DIN interrupts.
     * 
     * @param num DIN to attach interrupt.
     * @param callback Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     */
    void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode);
    
    /**
     * @brief Detach interrupt of a given DIN.
     * 
     * @param num DIN to detach interrupt.
     */
    void detachInterrupt(DIn_Num_t num);

private:

    BusCtrlCmd* _control;
    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif