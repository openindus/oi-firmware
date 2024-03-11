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

/* ADS114S08 Commands */
#define ADS114S0X_CMD_NOP           0x00
#define ADS114S0X_CMD_WAKEUP        0x02
#define ADS114S0X_CMD_POWERDOWN     0x04
#define ADS114S0X_CMD_RESET         0x06
#define ADS114S0X_CMD_START         0x08
#define ADS114S0X_CMD_STOP          0x0A
#define ADS114S0X_CMD_SYOCAL        0x16
#define ADS114S0X_CMD_SYGCAL        0x17
#define ADS114S0X_CMD_SFOCAL        0x19
#define ADS114S0X_CMD_RDATA         0x12
#define ADS114S0X_CMD_RREG          0x20
#define ADS114S0X_CMD_WREG          0x40

/* ADS114S08 Registers */
#define ADS114S0X_REG_ID            0x00
#define ADS114S0X_REG_STATUS        0x01
#define ADS114S0X_REG_INPMUX        0x02
#define ADS114S0X_REG_PGA           0x03
#define ADS114S0X_REG_DATARATE      0x04
#define ADS114S0X_REG_REF           0x05
#define ADS114S0X_REG_IDACMAG       0x06
#define ADS114S0X_REG_IDACMUX       0x07
#define ADS114S0X_REG_VBIAS         0x08
#define ADS114S0X_REG_SYS           0x09
#define ADS114S0X_REG_OFCAL0        0x0b
#define ADS114S0X_REG_OFCAL1        0x0c
#define ADS114S0X_REG_FSCAL0        0x0e
#define ADS114S0X_REG_FSCAL1        0x0f
#define ADS114S0X_REG_GPIODAT       0x10
#define ADS114S0X_REG_GPIOCON       0x11

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

/* Init */
int ads114s0x_init(ads114s0x_device_t** dev, ads114s0x_config_t* conf);

/* Control / Calibration */
int ads114s0x_wakeup(ads114s0x_device_t* dev);
int ads114s0x_powerdown(ads114s0x_device_t* dev);
int ads114s0x_reset(ads114s0x_device_t* dev);
int ads114s0x_start(ads114s0x_device_t* dev);
int ads114s0x_stop(ads114s0x_device_t* dev);
int ads114s0x_system_offset_calib(ads114s0x_device_t* dev);
int ads114s0x_system_gain_calib(ads114s0x_device_t* dev);
int ads114s0x_self_offset_calib(ads114s0x_device_t* dev);

#ifdef __cplusplus
}
#endif