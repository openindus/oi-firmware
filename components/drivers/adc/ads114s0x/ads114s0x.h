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

typedef struct __attribute__((packed)) {
    uint8_t dev_id : 3;
    uint8_t reserved : 5;
} ads114s0x_reg_id_t;

typedef struct __attribute__((packed)) {
    uint8_t fl_ref_l0 : 1;
    uint8_t fl_ref_l1 : 1;
    uint8_t fl_n_railn : 1;
    uint8_t fl_n_railp : 1;
    uint8_t fl_p_railn : 1;
    uint8_t fl_p_railp : 1;
    uint8_t rdy : 1;
    uint8_t fl_por : 1;
} ads114s0x_reg_status_t;

typedef struct __attribute__((packed)) {
    uint8_t muxn : 4;
    uint8_t muxp : 4;
} ads114s0x_reg_inpmux_t;

typedef struct __attribute__((packed)) {
    uint8_t gain : 3;
    uint8_t pga_en : 2;
    uint8_t delay : 3;
} ads114s0x_reg_pga_t;

typedef struct __attribute__((packed)) {
    uint8_t dr : 3;
    uint8_t filter : 1;
    uint8_t mode : 1;
    uint8_t clk : 1;
    uint8_t g_chop : 1;
} ads114s0x_reg_datarate_t;

typedef struct __attribute__((packed)) {
    uint8_t refcon : 2;
    uint8_t refsel : 2;
    uint8_t refn_buf : 1;
    uint8_t refp_buf : 1;
    uint8_t fl_ref_en : 2;
} ads114s0x_reg_ref_t;

typedef struct __attribute__((packed)) {
    uint8_t imag : 4;
    uint8_t unused : 2;
    uint8_t psw : 1;
    uint8_t fl_rail_en : 1;
} ads114s0x_reg_idacmag_t;

typedef struct __attribute__((packed)) {
    uint8_t i1mux : 4;
    uint8_t i2mux : 4;
} ads114s0x_reg_idacmux_t;

typedef struct __attribute__((packed)) {
    uint8_t vb_ain0 : 1;
    uint8_t vb_ain1 : 1;
    uint8_t vb_ain2 : 1;
    uint8_t vb_ain3 : 1;
    uint8_t vb_ain4 : 1;
    uint8_t vb_ain5 : 1;
    uint8_t vb_ainc : 1;
    uint8_t vb_level : 1;
} ads114s0x_reg_vbias_t;

typedef struct __attribute__((packed)) {
    uint8_t sendstat : 1;
    uint8_t crc : 1;
    uint8_t timeout : 1;
    uint8_t cal_samp : 2;
    uint8_t sys_mon : 3;
} ads114s0x_reg_sys_t;

typedef struct __attribute__((packed)) {
    uint16_t ofc;
} ads114s0x_reg_ofcal_t;

typedef struct __attribute__((packed)) {
    uint16_t fsc;
} ads114s0x_reg_fscal_t;

typedef struct __attribute__((packed)) {
    uint8_t dat : 4;
    uint8_t dir : 4;
} ads114s0x_reg_gpiodat_t;

typedef struct __attribute__((packed)) {
    uint8_t con : 4;
    uint8_t unused : 4;
} ads114s0x_reg_gpiocon_t;

typedef struct __attribute__((packed)) {
    ads114s0x_reg_id_t id;
    ads114s0x_reg_status_t status;
    ads114s0x_reg_inpmux_t inpmux;
    ads114s0x_reg_pga_t pga;
    ads114s0x_reg_datarate_t datarate;
    ads114s0x_reg_ref_t ref;
    ads114s0x_reg_idacmag_t idacmag;
    ads114s0x_reg_idacmux_t idacmux;
    ads114s0x_reg_vbias_t vbias;
    ads114s0x_reg_sys_t sys;
    uint8_t reserved1;
    ads114s0x_reg_ofcal_t ofcal;
    uint8_t reserved2;
    ads114s0x_reg_fscal_t fscal;
    ads114s0x_reg_gpiodat_t gpiodat;
    ads114s0x_reg_gpiocon_t gpiocon;
} ads114s0x_register_map_t;

/* HAL */

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