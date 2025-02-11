/**
 * @file Led.h
 * @brief 
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>

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
} LedState_t;

class Led
{
public:

    static void on(LedColor_t color);
    static void off(void);
    static void blink(LedColor_t color, uint32_t period);

};
