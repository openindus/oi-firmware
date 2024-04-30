/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Encoder.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdio.h>
#include <math.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "driver/pcnt.h"
#include "driver/gpio.h"

class Encoder
{
public:

    static int init(gpio_num_t codera, gpio_num_t coderb, int16_t ValueMaxPulsePerRevolution = 1000, int16_t ValueMinPulsePerRevolution = -1000);
    static bool getDirection(void);
    static float getSpeed(void);
    static float getPosition(void);

private:

    static QueueHandle_t _PcntEvtQueue;

    typedef struct {
        int unit;  
        uint32_t status; 
    } pcnt_evt_t;

    static SemaphoreHandle_t _xSemaphore;

    static int _position;
    static float _pos;
    static bool _direction;
    static float _speed;
    static float _pulsePerRevolution;
    static pcnt_unit_t _PcntUnit;
    static pcnt_evt_t evt;
    
    static void _pcntIntrHandler(void *pvParameters);
    static void _vTaskFunctionRevolution(void *pvParameters); 
};
