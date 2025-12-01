/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorBLDC.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "Motor.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <math.h>
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/pulse_cnt.h"

//definition PWM
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_9_BIT // Set duty resolution to 9 bits. To set duty to 50% --> ((2 ** 9) - 1) * 50% = 255 (vmax = 510)
#define LEDC_FREQUENCY          (45000) // Frequency in Hertz. Set frequency at 5 kHz

class MotorBLDC
{
public:

    static int init(gpio_num_t brake, gpio_num_t direction, gpio_num_t enable_chip, gpio_num_t speed_ctrl, gpio_num_t nfault, gpio_num_t fgout);
    static void setSpeed(uint32_t speed);
    static void setBrake(bool brake);
    static void setDirection(bool direction);
    static float getSpeed(void);

private:

    static QueueHandle_t _PcntEvtQueue1;

    typedef struct {
        int unit;  
        int watch_point_value; 
    } pcnt_evt_t;

    static SemaphoreHandle_t _xSemaphore;

    static float _speed;
    static pcnt_unit_handle_t _PcntUnit1;
    static pcnt_evt_t evt1;

    static gpio_num_t* _gpio;

    static void _ledcInit();
    static bool _pcntIntrHandler1(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx);
    static void _vTaskFunctionSpeed(void *pvParameters); 
};
