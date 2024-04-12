/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BusCAN.h
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

class BusCAN
{
public:

    typedef struct __attribute__((__packed__)) {
        uint8_t cmd;
        uint8_t data[7];
    } Frame_t;

    static void begin(gpio_num_t txNum, gpio_num_t rxNum);
    static void end(void);
    static int write(Frame_t* frame, uint16_t id, uint8_t length = 8);
    static int read(Frame_t* frame, uint16_t* id, uint8_t* length, TickType_t timeout = portMAX_DELAY);

private:

    static SemaphoreHandle_t _mutex;

};
