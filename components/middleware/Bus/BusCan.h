/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCan.h
 * @brief this class control the bus
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

class BusCan
{
public:

    typedef struct __attribute__((__packed__)) {
        uint8_t command;
        uint8_t event;
        uint16_t unused;
        uint32_t data;
    } Frame_t;

    static void begin(gpio_num_t txNum, gpio_num_t rxNum);
    static void end(void);
    static int write(Frame_t* frame, uint16_t id);
    static int read(Frame_t* frame, uint16_t* id, TickType_t timeout);

private:

    static SemaphoreHandle_t _mutex;

};
