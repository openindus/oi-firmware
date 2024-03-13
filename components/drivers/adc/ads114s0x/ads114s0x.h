/**
 * @file ads114s0x.h
 * @brief ADS114S0x ADC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    spi_device_handle_t spi_handler;
} ads114s0x_device_t;

typedef struct {
    spi_host_device_t host_id;
    int sclk_freq;
    gpio_num_t start_sync;
    gpio_num_t reset;
    gpio_num_t cs;
    gpio_num_t drdy;
} ads114s0x_config_t;

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

enum ads114s0x_dev_id_e {
    ADS114S0X_DEV_ID_ADS114S08 = 0b100, // ADS114S08 (12 channels, 16 bits)
    ADS114S0X_DEV_ID_ADS114S06 = 0b101  // ADS114S06 (6 channels, 16 bits)
};

/**
 * @brief Device ID Register (address = 00h)
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_dev_id_e dev_id  : 3;
    uint8_t reserved                : 5;
} ads114s0x_reg_id_t;

/**
 * @brief Device Status Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint8_t fl_ref_l0   : 1; // Reference voltage monitor flag lvl 0 [0: >= 0.3V, 1: <0.3V]
    uint8_t fl_ref_l1   : 1; // Reference voltage monitor flag lvl 1 [0: >= 1/3, 1: <1/3 (AVDD-AVSS)]
    uint8_t fl_n_railn  : 1; // Negative PGA output at negative rail flag [0: No error, 1: within 150 mV of AVSS]
    uint8_t fl_n_railp  : 1; // Negative PGA output at positive rail flag [0: No error, 1: within 150 mV of AVDD]
    uint8_t fl_p_railn  : 1; // Positive PGA output at negative rail flag [0: No error, 1: within 150 mV of AVSS]
    uint8_t fl_p_railp  : 1; // Positive PGA output at positive rail flag [0: No error, 1: within 150 mV of AVDD]
    uint8_t rdy         : 1; // Device ready flag [0: ADC ready for communication, 1: ADC not ready]
    uint8_t fl_por      : 1; // Power-on reset flag (lag must be cleared by user register write 1)
} ads114s0x_reg_status_t;

enum ads114s0x_adc_input_e {
    ADS114S0X_AIN0          = 0b0000,
    ADS114S0X_AIN1          = 0b0001,
    ADS114S0X_AIN2          = 0b0010,
    ADS114S0X_AIN3          = 0b0011,
    ADS114S0X_AIN4          = 0b0100,
    ADS114S0X_AIN5          = 0b0101,
    ADS114S0X_AIN6          = 0b0110,
    ADS114S0X_AIN7          = 0b0111,
    ADS114S0X_AIN8          = 0b1000,
    ADS114S0X_AIN9          = 0b1001,
    ADS114S0X_AIN10         = 0b1010,
    ADS114S0X_AIN11         = 0b1011,
    ADS114S0X_AINCOM        = 0b1100
};

/**
 * @brief Input Multiplexer Register
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_adc_input_e muxn : 4; // Negative ADC input 
    enum ads114s0x_adc_input_e muxp : 4; // Positive ADC input
} ads114s0x_reg_inpmux_t;

enum ads114s0x_pga_gain_e {
    ADS114S0X_PGA_GAIN_1        = 0b000,
    ADS114S0X_PGA_GAIN_2        = 0b001,
    ADS114S0X_PGA_GAIN_4        = 0b010,
    ADS114S0X_PGA_GAIN_8        = 0b011,
    ADS114S0X_PGA_GAIN_16       = 0b100,
    ADS114S0X_PGA_GAIN_32       = 0b101,
    ADS114S0X_PGA_GAIN_64       = 0b110,
    ADS114S0X_PGA_GAIN_128      = 0b111
};

enum ads114s0x_delay_e {
    ADS114S0X_DELAY_14_TMOD     = 0b000,
    ADS114S0X_DELAY_25_TMOD     = 0b001,
    ADS114S0X_DELAY_64_TMOD     = 0b010,
    ADS114S0X_DELAY_256_TMOD    = 0b011,
    ADS114S0X_DELAY_1024_TMOD   = 0b100,
    ADS114S0X_DELAY_2048_TMOD   = 0b101,
    ADS114S0X_DELAY_4096_TMOD   = 0b110,
    ADS114S0X_DELAY_1_TMOD      = 0b111
};

/**
 * @brief Gain Setting Register
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_pga_gain_e gain      : 3; // PGA gain
    uint8_t pga_en                      : 2; // PGA [0: disable, 1: enable]
    enum ads114s0x_delay_e delay        : 3; // PGA delay
} ads114s0x_reg_pga_t;

enum ads114s0x_data_rate_e {
    ADS114S0X_DATA_RATE_2_5_SPS     = 0b0000,
    ADS114S0X_DATA_RATE_5_SPS       = 0b0001,
    ADS114S0X_DATA_RATE_10_SPS      = 0b0010,
    ADS114S0X_DATA_RATE_16_6_SPS    = 0b0011,
    ADS114S0X_DATA_RATE_20_SPS      = 0b0100,
    ADS114S0X_DATA_RATE_50_SPS      = 0b0101,
    ADS114S0X_DATA_RATE_60_SPS      = 0b0110,
    ADS114S0X_DATA_RATE_100_SPS     = 0b0111,
    ADS114S0X_DATA_RATE_200_SPS     = 0b1000,
    ADS114S0X_DATA_RATE_400_SPS     = 0b1001,
    ADS114S0X_DATA_RATE_800_SPS     = 0b1010,
    ADS114S0X_DATA_RATE_1000_SPS    = 0b1011,
    ADS114S0X_DATA_RATE_2000_SPS    = 0b1100,
    ADS114S0X_DATA_RATE_4000_SPS    = 0b1101,
    ADS114S0X_DATA_RATE_8000_SPS    = 0b1110
};

/**
 * @brief Data Rate Register
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_data_rate_e dr   : 4; // Data rate
    uint8_t filter                  : 1; // Digital filter [0: sinc, 1: low latency] (default 1)
    uint8_t mode                    : 1; // Conversion mode [0: continuous, 1: single shot]
    uint8_t clk                     : 1; // Clock source [0: internal, 1: external]
    uint8_t g_chop                  : 1; // Global chop enable [0: disable, 1: enable]
} ads114s0x_reg_datarate_t;

enum ads114s0x_ref_config_e {
    ADS114S0X_REF_OFF           = 0b00,
    ADS114S0X_REF_ON            = 0b01,
    ADS114S0X_REF_ALWAYS_ON     = 0b10
};

enum ads114s0x_ref_selection_e {
    ADS114S0X_REF_REFP0_REFNO       = 0b00,
    ADS114S0X_REF_REFP1_REFN1       = 0b01,
    ADS114S0X_REF_INTERNAL_2_5V     = 0b10
};

/**
 * @brief Reference Control Register
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_ref_config_e refcon      : 2; // Internal voltage reference configuration
    enum ads114s0x_ref_selection_e refsel   : 2; // Reference input selection (ADC source)
    uint8_t refn_buf                        : 1; // Negative reference buffer bypass [0: enable, 1: disable]
    uint8_t refp_buf                        : 1; // Positive reference buffer bypass [0: enable, 1: disable]
    /** @todo */
    uint8_t fl_ref_en                       : 2; // Reference monitor configuration
} ads114s0x_reg_ref_t;

enum ads114s0x_idac_magnitude_e {
    ADS114S0X_IDAC_OFF              = 0b0000,
    ADS114S0X_IDAC_10_UA            = 0b0001,
    ADS114S0X_IDAC_50_UA            = 0b0010,
    ADS114S0X_IDAC_100_UA           = 0b0011,
    ADS114S0X_IDAC_250_UA           = 0b0100,
    ADS114S0X_IDAC_500_UA           = 0b0101,
    ADS114S0X_IDAC_750_UA           = 0b0110,
    ADS114S0X_IDAC_1000_UA          = 0b0111,
    ADS114S0X_IDAC_1500_UA          = 0b1000,
    ADS114S0X_IDAC_2000_UA          = 0b1001
};

/**
 * @brief Excitation Current Register
 * 
 */
typedef struct __attribute__((packed)) {
    enum ads114s0x_idac_magnitude_e imag    : 4; // Value of the excitation current source
    uint8_t reserved                        : 2;
    uint8_t psw                             : 1; // Low-side power switch [0: open, 1: closed]
    uint8_t fl_rail_en                      : 1; // PGA ouput rail flag enable
    enum ads114s0x_adc_input_e i1mux        : 4; // idac1 output channel
    enum ads114s0x_adc_input_e i2mux        : 4; // idac2 output channel
} ads114s0x_reg_idac_t;

/**
 * @brief Sensor Biasing Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint8_t vb_ain0     : 1; // AIN0 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ain1     : 1; // AIN1 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ain2     : 1; // AIN2 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ain3     : 1; // AIN3 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ain4     : 1; // AIN4 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ain5     : 1; // AIN5 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_ainc     : 1; // AIN6 VBIAS selection [0: disconnected, 1: connected]
    uint8_t vb_level    : 1; // VBIAS level selection [0: (AVDD+AVSS)/2 , 1: (AVDD+AVSS)/12]
} ads114s0x_reg_vbias_t;

enum ads114s0x_calib_sample_e {
    ADS114S0X_CALIB_SAMPLE_1    = 0b00,
    ADS114S0X_CALIB_SAMPLE_4    = 0b01,
    ADS114S0X_CALIB_SAMPLE_8    = 0b10,
    ADS114S0X_CALIB_SAMPLE_16   = 0b11
};

/**
 * @brief System Control Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint8_t sendstat                        : 1; // STATUS byte enable [0: disable, 1: enable]
    uint8_t crc                             : 1; // CRC enable [0: disable, 1: enable]
    uint8_t timeout                         : 1; // SPI timeout enable [0: disable, 1: enable]
    enum ads114s0x_calib_sample_e cal_samp  : 2; // Calibration sample size selection
    /** @todo */
    uint8_t sys_mon                         : 3; // System monitor configuration
} ads114s0x_reg_sys_t;

/**
 * @brief Offset Calibration Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint16_t ofc;
} ads114s0x_reg_ofcal_t;

/**
 * @brief Gain Calibration Register 
 * 
 */
typedef struct __attribute__((packed)) {
    uint16_t fsc;
} ads114s0x_reg_fscal_t;

/**
 * @brief GPIO Data Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint8_t dat : 4; // GPIO level (GPIO[x]) [0: low, 1: high]
    uint8_t dir : 4; // GPIO direction (GPIO[x]) [0: output, 1: input]
} ads114s0x_reg_gpiodat_t;

/**
 * @brief GPIO Configuration Register
 * 
 */
typedef struct __attribute__((packed)) {
    uint8_t con : 4; // GPIO pin configuration (GPIO[x]) [0: analog, 1: digital]
    uint8_t unused : 4;
} ads114s0x_reg_gpiocon_t;

/* Init */
int ads114s0x_init(ads114s0x_device_t** dev, ads114s0x_config_t* conf);

/* Control */
int ads114s0x_wakeup(ads114s0x_device_t* dev);
int ads114s0x_powerdown(ads114s0x_device_t* dev);
int ads114s0x_reset(ads114s0x_device_t* dev);
int ads114s0x_start(ads114s0x_device_t* dev);
int ads114s0x_stop(ads114s0x_device_t* dev);

/* Calibation */
int ads114s0x_system_offset_calib(ads114s0x_device_t* dev);
int ads114s0x_system_gain_calib(ads114s0x_device_t* dev);
int ads114s0x_self_offset_calib(ads114s0x_device_t* dev);

/* Read data */
int ads114s0x_read_data(ads114s0x_device_t* dev, uint16_t* data);

/* Read/Write registers */
int ads114s0x_read_register(ads114s0x_device_t* dev, uint8_t reg_addr, uint8_t* reg_data, size_t reg_size);
int ads114s0x_write_register(ads114s0x_device_t* dev, uint8_t reg_addr, uint8_t* reg_data, size_t reg_size);

#ifdef __cplusplus
}
#endif




