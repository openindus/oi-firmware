/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ioexpander.h
 * @brief Functions to the ioexpander pcal6524
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
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief ioexpander available pins
 * 
 */
typedef enum
{
    IOEX_PORT0 = 0,
    IOEX_PORT1 = 1,
    IOEX_PORT2 = 2,
} ioex_port_t;

/**
 * @brief ioexpander available pins
 * 
 */
typedef enum
{
    IOEX_NUM_NC     = -1,    /*!< Use to signal not connected to S/W */

    /*!< PORT0 */
    IOEX_NUM_0      = 0,
    IOEX_NUM_1      = 1,
    IOEX_NUM_2      = 2,
    IOEX_NUM_3      = 3,
    IOEX_NUM_4      = 4,
    IOEX_NUM_5      = 5,
    IOEX_NUM_6      = 6,
    IOEX_NUM_7      = 7,
    /*!< PORT1 */
    IOEX_NUM_10     = 8,
    IOEX_NUM_11     = 9,
    IOEX_NUM_12     = 10,
    IOEX_NUM_13     = 11,
    IOEX_NUM_14     = 12,
    IOEX_NUM_15     = 13,
    IOEX_NUM_16     = 14,
    IOEX_NUM_17     = 15,
    /*!< PORT2 */
    IOEX_NUM_20     = 16,
    IOEX_NUM_21     = 17,
    IOEX_NUM_22     = 18,
    IOEX_NUM_23     = 19,
    IOEX_NUM_24     = 20,
    IOEX_NUM_25     = 21,
    IOEX_NUM_26     = 22,
    IOEX_NUM_27     = 23,
    IOEX_NUM_MAX    = 24,
} ioex_num_t;

/**
 * @brief ioexpander level 
 * 
 */
typedef enum
{
    IOEX_LOW     = 0,   /*!< GPIO level LOW  */
    IOEX_HIGH    = 1,   /*!< GPIO level HIGH */
} ioex_level_t;

/**
 * @brief ioexpander direction/mode
 * 
 */
typedef enum
{
    IOEX_OUTPUT     = 0,     /*!< GPIO mode : input only  */
    IOEX_INPUT      = 1,     /*!< GPIO mode : output only */
} ioex_mode_t;

/**
 * @brief ioexpander available pull mode
 * 
 */ 
typedef enum {
    IOEX_PULLUP     = 0,      /*!< Pad pull up   */
    IOEX_PULLDOWN   = 1,      /*!< Pad pull down */
    IOEX_FLOATING   = 2,      /*!< Pad floating  */
} ioex_pull_mode_t;

/**
 * @brief ioexpander available drive strength -- /!\ NOT IMPLEMENTED YET /!\
 * 
 */
typedef enum {
    IOEX_DRIVE_CAP_0       = 0,    /*!< Pad drive capability: weak      */
    IOEX_DRIVE_CAP_1       = 1,    /*!< Pad drive capability: stronger  */
    IOEX_DRIVE_CAP_2       = 2,    /*!< Pad drive capability: medium    */
    IOEX_DRIVE_CAP_3       = 3,    /*!< Pad drive capability: strongest */
    IOEX_DRIVE_CAP_MAX,
} ioex_drive_cap_t;

/**
 * @brief ioexpander available interrupt mode
 *
 */
typedef enum
{
    IOEX_INTERRUPT_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    IOEX_INTERRUPT_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    IOEX_INTERRUPT_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    IOEX_INTERRUPT_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
} ioex_interrupt_type_t;

/**
 * @brief typedef for interrupt handler
 *
 */
typedef void (*ioex_isr_t)(void *);

/**
 * @brief linked list element for interrupt config
 *
 */
typedef struct ioex_interrupt_element_t ioex_interrupt_element_t;
struct ioex_interrupt_element_t
{
    ioex_num_t ioex_num;                /*!< IO num                                   */
    uint8_t priority;                   /*!< Priority of the interrupt                */
    ioex_isr_t isr_handler;             /*!< isr function call when interrupt occured */
    void *args;                         /*!< arguments of the isr function            */
    ioex_interrupt_element_t *next;     /*!< next element of the list                 */
};

/**
 * @brief linked list beginning for interrupt config
 *
 */
typedef struct ioex_interrupt_list_t ioex_interrupt_list_t;
struct ioex_interrupt_list_t
{
    ioex_interrupt_element_t *first;
    uint8_t count;
};

/**
 * @brief Configuration parameters of IO pad for ioex_config function
 *
 */
typedef struct {
    uint32_t pin_bit_mask;                                      /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    ioex_mode_t mode;                                           /*!< GPIO mode: set input/output mode                     */
    ioex_pull_mode_t pull_mode;                                 /*!< GPIO pull-up                                         */
    ioex_interrupt_type_t interrupt_type;                       /*!< GPIO interrupt type                                  */
} ioex_config_t;

/**
* @brief Declare of ioex_device Type
*
*/
typedef struct 
{    
    uint8_t address;                               /*<! address of the ioexpander         */
    uint8_t i2c_port;                              /*<! i2c port of the ioexpander        */
    TaskHandle_t interrupt_handle;                 /*<! ioexpander interrupt task handle  */
    gpio_num_t interrupt_pin;                      /*<! interrupt pin of the ioexpander   */
    ioex_interrupt_list_t *interrupt_list;         /*!< ISR Struct for interrupt          */
} ioex_device_t;

/**
 * @brief Create an instance of ioexpander and initialize it
 *
 * @param[in] i2c_port: i2c port of the i2c configured
 * @param[in] i2c_address: i2c address of the ioexpander
 * @param[in] use_interrupt: set to true if interrupt management is required
 * @param[in] interrupt_num: gpio use for interrupt; ignored is use_interrupt is set to false
 *
 * @note i2c must be initialize with the good port before calling this function
 *
 * @return ioexpader_pca6524 instance or NULL if an error occured (ex: i2c not configured)
 */
ioex_device_t *ioex_create(i2c_port_t i2c_port, uint8_t i2c_address, bool use_interrupt, gpio_num_t interrupt_pin);

/**
 * @brief Uninstall led driver
 * 
 * @param[in] io: instance of ioexpander
 * @return ESP_OK or ESP_FAIL
 *
 */
esp_err_t ioex_delete(ioex_device_t *ioex_device);


/**
 * @brief ioex common configuration:  Configure IO's Mode,pull-up,PullDown,IntrType
 *
 * @param[in] io: instance of ioexpander
 * @param[in] pIOEXConfig Pointer to GPIO configure struct
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t ioex_config(ioex_device_t *io, const ioex_config_t *pIOEXConfig);

/**
 * @brief Set a pin level
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 * @param[in] level: level
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_set_level(ioex_device_t *io, ioex_num_t ioex_num, ioex_level_t level);

esp_err_t ioex_set_level_multiple(ioex_device_t *io,  ioex_port_t port, uint8_t pins, ioex_level_t level);

/**
 * @brief Get a pin level
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 *
 * @return
 *      -  1: HIGH
 *      -  2: LOW
 *      - -1: ERROR
 */
int ioex_get_level(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief Set a pin direction
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin numbers
 * @param[in] direction: direction
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_set_direction(ioex_device_t *io, ioex_num_t ioex_num, ioex_mode_t mode);

esp_err_t ioex_set_direction_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_mode_t mode);

/**
 * @brief Set a pin pull mode
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 * @param[in] pull_mode: pull mode 
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_set_pull_mode(ioex_device_t *io, ioex_num_t ioex_num, ioex_pull_mode_t pull_mode);

esp_err_t ioex_set_pull_mode_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_pull_mode_t pull_mode);

/**
 * @brief Enable pullup on a pin
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_pullup_enable(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief Enable pulldown on a pin
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_pulldown_enable(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief Disiable pulldown on a pin
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num: pin number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_pull_disable(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief  ioex set interrupt trigger type
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num ioex number
 * @param[in] interrupt_type Interrupt type, select from ioex_interrupt_type_t
 *
 * @return
 *      - ESP_OK  Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_set_interrupt_type(ioex_device_t *io, ioex_num_t ioex_num, ioex_interrupt_type_t interrupt_type);

esp_err_t ioex_set_interrupt_type_multiple(ioex_device_t *io, ioex_port_t port, uint8_t pins, ioex_interrupt_type_t interrupt_type);

/**
 * @brief  ioex enable interrupt signal
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num ioex number
 *
 * @return
 *      - ESP_OK  Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_interrupt_enable(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief  ioex disable interrupt signal
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num ioex number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_interrupt_disable(ioex_device_t *io, ioex_num_t ioex_num);

/**
 * @brief  ioex add handler to io interrupt with given priority
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num ioex number
 * @param[in] isr_handler handler function
 * @param[in] args handler arguments
 * @param[in] priority priority of the interrupt
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_isr_handler_add(ioex_device_t *io, ioex_num_t ioex_num, ioex_isr_t isr_handler, void *args, uint8_t priority);

/**
 * @brief  ioex delete handler and disable interrupt
 *
 * @param[in] io: instance of ioexpander
 * @param[in] ioex_num ioex number
 *
 * @return
 *      - ESP_OK Success
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_FAIL i2c error
 */
esp_err_t ioex_isr_handler_remove(ioex_device_t *io, ioex_num_t ioex_num);

#ifdef __cplusplus
}
#endif
