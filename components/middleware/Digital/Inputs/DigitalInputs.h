/**
 * @file DigitalInputs.h
 * @brief Digital Inputs
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "DigitalInputsTypes.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

class DigitalInputs
{
protected:
    static int init(const gpio_num_t *gpio, int nb);

public:
    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     *
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0).
     */
    static int digitalRead(DIn_Num_t num);

    template <typename T, typename R> 
    static inline R digitalRead(T num) {
        return static_cast<R>(digitalRead((DIn_Num_t)num));
    }

    /**
     * @brief Attach a user callback to the DIN interrupts.
     *
     * @param num DIN to attach interrupt.
     * @param callback Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     * @param arg argument for the handler
     */
    static void attachInterrupt(DIn_Num_t num, IsrCallback_t callback, InterruptMode_t mode,
                                void *arg = NULL);

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
    static gpio_num_t *_gpio_nums;

    static IsrCallback_t *_callbacks;
    static void **_args;

    static QueueHandle_t _event;
    static void IRAM_ATTR _isr(void *pvParameters);
    static void _task(void *pvParameters);

    static int _registerCLI(void);
};
