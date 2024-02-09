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

class DigitalInput
{
public:

    DigitalInput(gpio_num_t gpio[DIN_MAX], int num) {
        memset(_gpio, GPIO_NUM_NC, DIN_MAX * sizeof(gpio_num_t));
        memcpy(_gpio, gpio, num * sizeof(gpio_num_t));
    }

    void init(void);
    int digitalRead(DigitalInputNum_t din);
    void attachInterrupt(DigitalInputNum_t din, IsrCallback_t callback, InterruptMode_t mode, void* arg);
    void detachInterrupt(DigitalInputNum_t din);

private:

    static gpio_num_t _gpio[DIN_MAX];
    static IsrCallback_t _callback[DIN_MAX];
    static void* _arg[DIN_MAX];

    static xQueueHandle _event;

    static void IRAM_ATTR _isr(void* arg);
    static void _task(void* arg);

};
