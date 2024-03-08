/**
 * @file ads114s0x.h
 * @brief ADS114S0x ADC driver
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

/* ADS114S08 Registers */
#define ADS114S0X_REG_ID        0x00
#define ADS114S0X_REG_STATUS    0x01
#define ADS114S0X_REG_INPMUX    0x02
#define ADS114S0X_REG_PGA       0x03
#define ADS114S0X_REG_DATARATE  0x04
#define ADS114S0X_REG_REF       0x05
#define ADS114S0X_REG_IDACMAG   0x06
#define ADS114S0X_REG_IDACMUX   0x07
#define ADS114S0X_REG_VBIAS     0x08
#define ADS114S0X_REG_SYS       0x09
#define ADS114S0X_REG_OFCAL0    0x0b
#define ADS114S0X_REG_OFCAL1    0x0c
#define ADS114S0X_REG_FSCAL0    0x0e
#define ADS114S0X_REG_FSCAL1    0x0f
#define ADS114S0X_REG_GPIODAT   0x10
#define ADS114S0X_REG_GPIOCON   0x11

typedef struct {
    spi_host_device_t host_id;
    int sclk_freq;
    gpio_num_t start_sync;
    gpio_num_t reset;
    gpio_num_t cs;
    gpio_num_t drdy;
} ads114s0x_config_t;

typedef struct {
    spi_device_handle_t spi_handler;
} ads114s0x_device_t;

int ads114s0x_init(ads114s0x_device_t** dev, ads114s0x_config_t* conf);

#ifdef __cplusplus
}
#endif