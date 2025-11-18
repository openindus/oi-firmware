/**
 * @file DigitalInputsInterface.h
 * @brief Digital Inputs Interface
 * @author Kevin Lefeuvre (kevinLefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Common.h"

/**
 * @brief Digital Inputs Numbers
 *
 */
typedef enum {
    DIN_1 = 0,
    DIN_2,
    DIN_3,
    DIN_4,
#if defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DC) || defined(CONFIG_MODULE_MASTER)
    DIN_5,
    DIN_6,
    DIN_7,
    DIN_8,
#elif defined(CONFIG_OI_DISCRETE) || defined(CONFIG_MODULE_MASTER)
    DIN_9,
    DIN_10,
#endif
    DIN_MAX
} DIn_Num_t;

/**
 * @brief Digital Inputs Logic
 *
 */
typedef enum { 
    ACTIVE_LOW = 0, 
    ACTIVE_HIGH = 1 
} Logic_t;

/**
 * @brief Digital Inputs Interrupts Modes
 *
 */
typedef enum { 
    NONE_MODE = 0, 
    RISING_MODE, 
    FALLING_MODE, 
    CHANGE_MODE 
} InterruptMode_t;

/**
 * @brief Function prototype for attachInterrupt callbacks
 *
 */
typedef void (*IsrCallback_t)(void *);

/**
 * @brief Digital Inputs class
 *
 */
class DigitalInputsInterface
{
public:
    /**
     * @brief Virtual destructor for DigitalInputsInterface
     */
    virtual ~DigitalInputsInterface() {}

    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     *
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0).
     */
    virtual int digitalRead(DIn_Num_t num) = 0;

    /**
     * @brief Attach a user callback to the DIN interrupts.
     *
     * @param num DIN to attach interrupt.
     * @param callback Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     * @param arg argument for the handler
     */
    virtual void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode, void *arg = NULL) = 0;

    /**
     * @brief Detach interrupt of a given DIN.
     *
     * @param num DIN to detach interrupt.
     */
    virtual void detachInterrupt(DIn_Num_t num) = 0;
};
