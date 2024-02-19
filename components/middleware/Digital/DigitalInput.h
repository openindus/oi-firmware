/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DigitalInput.h
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

class DigitalInput
{
public:

    DigitalInput(const gpio_num_t *gpio, int num);
    DigitalInput(ioex_device_t *ioex, const ioex_num_t *ioex_num, int num);
    ~DigitalInput();

    void init(void);
    int digitalRead(DigitalInputNum_t din);
    void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode, void* arg);
    void detachInterrupt(DigitalInputNum_t din);

private:

    /* Type of DOUT (gpio or ioex) */
    DigitalInputType_t _type;

    /* Number of DOUT */
    uint8_t _num; 
    
    /* GPIO num for DOUT (can be initialized as esp gpio or ioexpander gpio)*/
    gpio_num_t* _gpio_num;
    ioex_num_t* _ioex_num;

    IsrCallback_t* _callback;
    void** _arg;

    /* Stor a local copy of the pointer to an initilized ioex_device_t */
    ioex_device_t* _ioex;

    static xQueueHandle _event;
    static void IRAM_ATTR _isr(void* pvParameters);
    static void _task(void* pvParameters);

    /* Utils functions */
    int _common_get_level(DigitalInputNum_t dout);


};
