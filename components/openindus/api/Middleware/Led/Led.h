/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Led.h
 * @brief Functions to control a one wire led
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_RED_MASK    0x01
#define LED_GREEN_MASK  0x02
#define LED_BLUE_MASK   0x04

#define LED_ACTION_ON       0x00
#define LED_ACTION_OFF      0x01
#define LED_ACTION_BLINK    0x02

typedef enum {
    LED_NONE =      (uint8_t) 0x00,
	LED_RED =       (uint8_t) 0x01,
	LED_GREEN =     (uint8_t) 0x02,
	LED_YELLOW =    (uint8_t) 0x03, // RED and GREEN
	LED_BLUE =      (uint8_t) 0x04,
	LED_PURPLE =    (uint8_t) 0x05, // RED and BLUE
	LED_CYAN =      (uint8_t) 0x06, // BLUE and GREEN
	LED_WHITE =     (uint8_t) 0x07  // RED, BLUE and GREEN
} LedColor_t;

typedef enum {
    LED_ON      = (uint8_t) 0x00,
    LED_OFF     = (uint8_t) 0x01,
    LED_BLINK   = (uint8_t) 0x02,
    LED_SYNC    = (uint8_t) 0x03,
} LedState_t;

class Led
{
public:

    typedef struct {
        LedColor_t color;
        uint32_t period;
        bool blink;
        bool blinkStatus;
        bool installed;
        uint8_t intensity;
    } Control_t;

    /**
     * @brief install ledonewire
     * 
     */
    static void install(gpio_num_t gpio_num);

    /**
     * @brief remove led instance
     * 
     */
    static void remove(void);

    /**
     * @brief set led on with given color
     * 
     * @param[in] color: led color
     *
     */
    static void on(LedColor_t color);

    /**
     * @brief turn off led
     * 
     */
    static void off(void);


    /**
     * @brief blink led
     * 
     */
    static void blink(LedColor_t color, uint32_t period);

    /**
     * @brief resync led state
     * 
     */
    static void sync(void);

private:

    /* Config */
    static rmt_channel_handle_t _rmt_channel_handle; // RMT channel handle for TX
    static rmt_encoder_handle_t _led_encoder; // LED encoder handle
    static uint8_t _buffer[3]; // buffer for led color
    static gpio_num_t _gpio_num;
    static Control_t _control;
    static TaskHandle_t _taskHandle;

    static void _task(void *pvParameters);

    static int _registerCLI(void);

};

#ifdef __cplusplus
}
#endif
