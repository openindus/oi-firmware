/**
 * @file DigitalInputsCmd.h
 * @brief Digital Inputs commands
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "DigitalInputs.h"
#include "Master.h"
#include "Slave.h"

class DigitalInputsCmd
{
public:
    DigitalInputsCmd(ModuleControl *module) : _module(module), _isrCallback{NULL} {}

    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     *
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0).
     */
    int digitalRead(DIn_Num_t num);

    template <typename T, typename R> inline R digitalRead(T num) {
        return static_cast<R>(digitalRead((DIn_Num_t)num));
    }

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
    ModuleControl *_module;
    IsrCallback_t _isrCallback[DIN_MAX];
};

#endif