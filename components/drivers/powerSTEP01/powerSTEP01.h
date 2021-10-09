/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Project.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file powerSTEP01.h
 * @brief Drivers for powerSTEP01 component
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "STM32DriversComponents/powerstep01.h"

typedef enum {
    DEVICE1 = 0,
    DEVICE2 = 1,
    DEVICE3 = 2,
    NUMBER_OF_DEVICES,
} Powerstep01_Device_t;

typedef struct
{
    spi_host_device_t spi_host;
    int spi_freq;

    int spi_pin_mosi;
    int spi_pin_miso;
    int spi_pin_clk;
    int spi_pin_cs;

    gpio_num_t pin_busy_sync;
    gpio_num_t pin_flag;

    int num_of_devices;

    gpio_num_t pin_sw[NUMBER_OF_DEVICES];
    gpio_num_t pin_stby_rst[NUMBER_OF_DEVICES];

    int pwm_pin_stck;

    ledc_timer_t pwm_timer;
    ledc_mode_t pwm_mode;
    ledc_channel_t pwm_channel;
    
} Powerstep01_DeviceConfig_t;


#ifdef __cplusplus
extern "C"
{
#endif

static const char PS01_TAG[] = "powerSTEP01";

/* Custom OpenIndus functions */

/**
 * @brief Configure powerSTEP01 devices
 * @param config SPI and GPIOs configuration
 * @return void
 */
void Powerstep01_DeviceConfig(Powerstep01_DeviceConfig_t *config);

/**
 * @brief Set switch level
 * 
 * @param deviceId 
 * @param level 
 */
void Powerstep01_SetSwitchLevel(uint8_t deviceId, uint32_t level);

/**
 * @brief Init device with defaults parameters
 * 
 * @param deviceId
 */
void Powerstep01_InitDevice(uint8_t deviceId);

/**
 * @brief Init NVS with defaults parameters
 * 
 * @param deviceId 
 */
void Powerstep01_InitNVSParameters(uint8_t deviceId);

/**
 * @brief Set device config in NVS for given register
 * 
 * @param deviceId 
 * @param register
 * @param value
 */
void Powerstep01_SetMotorConfigToNVS(uint8_t deviceId, uint8_t register, uint16_t value);

#ifdef __cplusplus
}
#endif