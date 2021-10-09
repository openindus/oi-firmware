/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ledonewire.h
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
#include "driver/rmt.h"


/************************************************************************************************************/
/*------------------------------------------ HIGH LEVEL DRIVER ---------------------------------------------*/
/************************************************************************************************************/

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

typedef struct
{
    LedColor_t color;
    uint32_t period;
    bool blink;
    bool blinkStatus;
    bool installed;
    uint8_t intensity;
} ledonewire_control_t;

/**
 * @brief install ledonewire
 * 
 */
void ledonewire_install(gpio_num_t gpio_num);

/**
 * @brief remove led instance
 * 
 */
void ledonewire_remove();

/**
 * @brief set led on with given color
 * 
 * @param[in] color: led color
 *
 */
void ledonewire_on(LedColor_t color);

/**
 * @brief turn off led
 * 
 */
void ledonewire_off(void);



/**
 * @brief blink led
 * 
 */
void ledonewire_blink(LedColor_t color, uint32_t period);


/************************************************************************************************************/
/*------------------------------------------- LOW LEVEL DRIVER ---------------------------------------------*/
/************************************************************************************************************/


/**
* @brief ledonewire Type
*
*/
typedef struct
{
    /**
     * @brief RMT channel used to drive led
     * 
     */
    rmt_channel_t rmt_channel;

    /**
     * @brief buffer for led color
     * 
     */
    uint8_t buffer[3];
} ledonewire_t;

/**
 * @brief LED initialization parameter
 * 
 */
typedef struct
{
    gpio_num_t gpio_num;   /*!< GPIO num */
    rmt_channel_t channel; /*!< RMT Channel */
} ledonewire_config_t;

/**
 * @brief Default configuration for Ledonewire
 *
 */
#define LEDONEWIRE_DEFAULT_CONFIG(gpio) \
    {                                   \
        .gpio_num = gpio,               \
        .channel = RMT_CHANNEL_0,       \
    }

/**
 * @brief Install led driver,initialize rmt and create an instance of ledonewire
 *
 * @param[in] config: ledonewire config 
 * @return 
 *      - Ledonewire instance: Success 
 *      - NULL: Error
 */
ledonewire_t *ledonewire_create(const ledonewire_config_t *config);

/**
 * @brief Uninstall led driver
 * 
 * @param[in] ledonewire: instance of ledonewire
 * 
 * @return 
 *      - ESP_OK: Success
 */
esp_err_t ledonewire_delete(ledonewire_t *ledonewire);

/**
 * @brief Install led driver and initialize rmt
 *
 * @param[in] ledonewire: instance of ledonewire
 * @param[in] red: red color of the led (0..255)
 * @param[in] green: green color of the led (0..255)
 * @param[in] blue: blue color of the led  (0..255)
 *
 * @return 
 *     - ESP_OK: Success
 *     - ESP_TIMEOUT: Error if timeout occured
 */
esp_err_t ledonewire_set_color(ledonewire_t *ledonewire, uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif
