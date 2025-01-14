/**
 * @file DigitalInputsExp.h
 * @brief Digital input expander
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "DigitalInputs.h"

class DigitalInputsExp
{
protected:

    static int init(ioex_device_t **device, const ioex_num_t *num, int nb);

public:

    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    static int digitalRead(DIn_Num_t num);

    /**
     * @brief Attach a user callback to the DIN interrupts.
     * 
     * @param num DIN to attach interrupt.
     * @param callback Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     * @param arg argument for the handler
     */
    static void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg = NULL);
    
    /**
     * @brief Detach interrupt of a given DIN.
     * 
     * @param num DIN to detach interrupt.
     */
    static void detachInterrupt(DIn_Num_t num);

private:

    /* Number of DIN */
    static uint8_t _nb; 
    
    /* GPIO num for DIN */
    static ioex_num_t* _ioex_nums;

    static IsrCallback_t* _callbacks;
    static void** _args;

    /* Stor a local copy of the pointer to an initalized ioex_device_t */
    static ioex_device_t** _device;

    static QueueHandle_t _event;
    static void IRAM_ATTR _isr(void* pvParameters);
    static void _task(void* pvParameters);
};
