/**
 * @file ad5413.h
 * @brief AD5413 DAC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize the device
 * 
 * @param host_id SPI host ID
 * @param cs Chip select (SYNC)
 */
void ad5413_init(spi_host_device_t host_id, gpio_num_t cs);

#ifdef __cplusplus
}
#endif