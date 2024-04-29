/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInputs.h
 * @brief Functions for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <cstring>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "pcal6524.h"

typedef void (*IsrCallback_t)(void*);

typedef enum {
    DIN_1 = 0,
    DIN_2,
    DIN_3,
    DIN_4,
    DIN_5,
    DIN_6,
    DIN_7,
    DIN_8,
    DIN_9,
    DIN_10,
    DIN_MAX
} DigitalInputNum_t;

typedef enum {
    ACTIVE_LOW = 0,
    ACTIVE_HIGH = 1
} DigitalInputLogic_t;

typedef enum {
    NONE_MODE = 0,
    RISING_MODE,
    FALLING_MODE,
    CHANGE_MODE
} InterruptMode_t;

typedef enum {
    DIGITAL_INPUT_GPIO = 0,
    DIGITAL_INPUT_IOEX
} DigitalInputType_t;

class DigitalInputs
{
public:

    static void init(const gpio_num_t *gpio, int num);
    static void init(ioex_device_t **ioex, const ioex_num_t *ioex_num, int num);

    /**
     * @brief Read an input level. Argument is the DIN to read.
     * The function return an integer that correspond to the value of the DIN.
     * 
     * @param num DIN to monitor.
     * @return Value of the DIN input (1 or 0). 
     */
    static int digitalRead(DigitalInputNum_t num);

    /**
     * @brief Attach a user callback to the DIN interrupts.
     * 
     * @param num DIN to attach interrupt.
     * @param callback Handler to execute when an interrupt occurs.
     * @param mode Interrupt mode (RISING, FALLING or CHANGE)
     * @param arg argument for the handler
     */
    static void attachInterrupt(DigitalInputNum_t num, IsrCallback_t callback, InterruptMode_t mode, void* arg);
    
    /**
     * @brief Detach interrupt of a given DIN.
     * 
     * @param num DIN to detach interrupt.
     */
    static void detachInterrupt(DigitalInputNum_t num);

private:

    /* Type of DIN (gpio or ioex) */
    static DigitalInputType_t _type;

    /* Number of DIN */
    static uint8_t _num; 
    
    /* GPIO num for DIN (can be initialized as esp gpio or ioexpander gpio)*/
    static gpio_num_t* _gpio_nums;
    static ioex_num_t* _ioex_nums;

    static IsrCallback_t* _callbacks;
    static void** _args;

    /* Stor a local copy of the pointer to an initalized ioex_device_t */
    static ioex_device_t** _ioex;

    static xQueueHandle _event;
    static void IRAM_ATTR _isr(void* pvParameters);
    static void _task(void* pvParameters);
};
