#pragma once

#include <string.h>
#include <math.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VREF            (float)2.5

#define SPI_CLK 1000000
#define SPI_TIMEOUT pow(2, 15) / SPI_CLK
#define HOST_ID SPI2_HOST

/* ADC commands */
#define WAKEUP      0x02
#define POWERDOWN   0x04
#define RESET       0x06
#define START       0x08
#define STOP        0x0A
#define SYOCAL      0x16
#define SYGCAL      0x17
#define SFOCAL      0x19
#define RDATA       0x12

typedef struct{
    uint8_t ads1x4s08_fl_ref_l0   : 1;     // Reference voltage monitor flag, level 0 : bit 0
    uint8_t ads1x4s08_fl_ref_l1   : 1;     // Refrence voltage monitor flag, level 1 : bit 1
    uint8_t ads1x4s08_fl_n_railn  : 1;    // Negative PGA output at negative rail flag : bit 2
    uint8_t ads1x4s08_fl_n_railp  : 1;    // Negative PGA output at positive rail flag : bit 3
    uint8_t ads1x4s08_fl_p_railn  : 1;    // Positve PGA output at negative rail flag : bit 4
    uint8_t ads1x4s08_fl_p_railp  : 1;    // Positive PGA output at positive rail flag : bit 5
    uint8_t ads1x4s08_rdy         : 1;           // Device ready flag : bit 6
    uint8_t ads1x4s08_fl_por      : 1;        // POR flag : bit 7
}ads1x4s08_status_field_t;

typedef struct{
    uint8_t ads1x4s08_muxn        : 4;        // Negative ADC input selection : bit 3-0        
    uint8_t ads1x4s08_muxp        : 4;        // Positive ADC input selection : bit 7-4
}ads1x4s08_inpmux_field_t;

typedef struct{
    uint8_t ads1x4s08_gain        : 3;        // PGA gain selection : bit 2-0
    uint8_t ads1x4s08_pga_en      : 2;      // PGA enable : bit 4-3
    uint8_t ads1x4s08_delay       : 3;       // Programmable conversion delay : bit 7-5
}ads1x4s08_pga_field_t;

typedef struct{
    uint8_t dr          : 4;             // Data rate selection : bit 3-0
    uint8_t filter      : 1;         // Digital filter selection : bit 4
    uint8_t mode        : 1;           // Converison mode selection : bit 5
    uint8_t clk         : 1;            // Clock source selection : bit 6
    uint8_t g_chop      : 1;         // Global chop enable : bit 7
}ads1x4s08_datarate_field_t;

typedef struct {
    uint8_t ads1x4s08_refcon      : 2;         // Internal voltage reference configuration : bit 1-0
    uint8_t ads1x4s08_refsel      : 2;         // Reference input selection : bit 3-2
    uint8_t ads1x4s08_refn_buf    : 1;       // Negative reference buffer bypass : bit 4
    uint8_t ads1x4s08_refp_buf    : 1;       // Positive reference buffer bypass : bit 5
    uint8_t ads1x4s08_fl_ref_en   : 2;      // Reference monitor configuration : bit 7-6
}ads1x4s08_ref_field_t;

typedef struct {
    uint8_t ads1x4s08_imag        : 4;           // IDAC magnitude selection : bit 3-0        
    uint8_t ads1x4s08_reserved    : 2;       // Reserved bit field : bit 5-4        
    uint8_t ads1x4s08_psw         : 1;            // Low-side power switch : bit 6        
    uint8_t ads1x4s08_fl_rail_en  : 1;     // PGA output rail flag enable : bit 7
}ads1x4s08_idacmag_field_t;

typedef struct {
    uint8_t ads1x4s08_i1mux       : 4;       // IDAC1 output channel selection : bit 3-0
    uint8_t ads1x4s08_i2mux       : 4;       // IDAC2 output channel selection : bit 7-4
}ads1x4s08_idacmux_field_t;

typedef struct {
    uint8_t ads1x4s08_vb_ain0     : 1;        // AIN0 VBIAS selection : bit 0
    uint8_t ads1x4s08_vb_ain1     : 1;        // AIN1 VBIAS selection : bit 1
    uint8_t ads1x4s08_vb_ain2     : 1;        // AIN2 VBIAS selection : bit 2
    uint8_t ads1x4s08_vb_ain3     : 1;        // AIN3 VBIAS selection : bit 3
    uint8_t ads1x4s08_vb_ain4     : 1;        // AIN4 VBIAS selection : bit 4
    uint8_t ads1x4s08_vb_ain5     : 1;        // AIN5 VBIAS selection : bit 5
    uint8_t ads1x4s08_vb_ainc     : 1;        // AINCOM VBIAS selection : bit 6
    uint8_t ads1x4s08_vb_level    : 1;       // VBIAS level selection : bit 7        
}ads1x4s08_vbias_field_t;

typedef struct{
    uint8_t ads1x4s08_sendstat    : 1;       // STATUS byte enable : bit 0
    uint8_t ads1x4s08_crc         : 1;            // CRC enable : bit 1
    uint8_t ads1x4s08_timeout     : 1;        // SPI timeout enable : bit 2
    uint8_t ads1x4s08_cal_samp    : 2;    // Calibration sample size selection : bit 4-3
    uint8_t ads1x4s08_sys_mon     : 3;     // System monitor configuration : bit 7-5
}ads1x4s08_sys_field_t;

typedef struct{
    uint8_t ads1x4s08_dat         : 4;         // GPIO data : bit 3-0
    uint8_t ads1x4s08_dir         : 4;         // GPIO direction : bit 7-4
}ads1x4s08_gpiodat_field_t;

typedef struct {
    uint8_t ads1x4s08_con         : 4;            // GPIO pin configuration : bit 3-0
    uint8_t ads1x4s08_reserved    : 4;       // Reserved bit field :  bit 7-4
}ads1x4s08_gpiocon_field_t;

typedef struct {
    uint8_t ads1x4s08_device_id;                  // Device ADS1X4S08_ID register
    ads1x4s08_status_field_t ads1x4s08_status;        // Device status register
    ads1x4s08_inpmux_field_t ads1x4s08_inpmux;        // Input multiplexer register
    ads1x4s08_pga_field_t ads1x4s08_pga;              // Gain setting register
    ads1x4s08_datarate_field_t ads1x4s08_datarate;    // Data rate register
    ads1x4s08_ref_field_t ads1x4s08_ref;              // Reference control register
    ads1x4s08_idacmag_field_t ads1x4s08_idacmag;      // Excitation current register 1
    ads1x4s08_idacmux_field_t ads1x4s08_idacmux;      // Excitation current register 2
    ads1x4s08_vbias_field_t ads1x4s08_vbias;          // Sensor biasing register
    ads1x4s08_sys_field_t ads1x4s08_sys;              // System control register
    uint8_t ads1x4s08_ofcal0;                     // Offset calibration register 1
    uint8_t ads1x4s08_ofcal1;                     // Offset calibartion register 2
    uint8_t ads1x4s08_fscal0;                     // Gain calibration register 1
    uint8_t ads1x4s08_fscal1;                     // Gain calibration register 2
    ads1x4s08_gpiodat_field_t ads1x4s08_gpiodat;      // GPIO data
    ads1x4s08_gpiocon_field_t ads1x4s08_gpiocon;      // GPIO configuration
}ads1x4s08_map_register_t;

typedef struct{
    uint8_t ads1x4s08_status;
    uint8_t ads1x4s08_data1;
    uint8_t ads1x4s08_data2;
    #if DATA_FORMAT
        uint8_t ads1x4s08_data3;
    #endif
    uint8_t ads1x4s08_crc;
}ads1x4s08_data_t;

// extern ads1x4s08_data_t data;
extern ads1x4s08_map_register_t ads1x4s08_mapreg;

typedef enum{
    ADS1X4S08_SPI_MODE_0 = 0,     // CPOL = 0, CPHA = 0
    ADS1X4S08_SPI_MODE_1 = 1,     // CPOL = 0, CPHA = 1
    ADS1X4S08_SPI_MODE_2 = 2,     // CPOL = 1, CPHA = 1
    ADS1X4S08_SPI_MODE_3 = 3      // CPOL = 1, CPHA = 1
}ads1x4s08_spi_mode_t;

typedef enum{
    ADS1X4S08_REG_ID          = 0x00,
    ADS1X4S08_REG_STATUS      = 0x01,
    ADS1X4S08_REG_INPMUX      = 0x02,
    ADS1X4S08_REG_PGA         = 0x03,
    ADS1X4S08_REG_DATARATE    = 0x04,
    ADS1X4S08_REG_REF         = 0x05,
    ADS1X4S08_REG_IDACMAG     = 0x06,
    ADS1X4S08_REG_IDACMUX     = 0x07,
    ADS1X4S08_REG_VBIAS       = 0x08,
    ADS1X4S08_REG_SYS         = 0x09,
    ADS1X4S08_REG_OFCAL0      = 0x0b,
    ADS1X4S08_REG_OFCAL1      = 0x0c,
    ADS1X4S08_REG_FSCAL0       = 0x0e,
    ADS1X4S08_REG_FSCAL1      = 0x0f,
    ADS1X4S08_REG_GPIODAT     = 0x10,
    ADS1X4S08_REG_GPIOCON     = 0x11
}ads1x4s08_reg_addr_t;


typedef enum{
    ADS1X4S08_AIN0            = 0,
    ADS1X4S08_AIN1            = 1,
    ADS1X4S08_AIN2            = 2,
    ADS1X4S08_AIN3            = 3,
    ADS1X4S08_AIN4            = 4,
    ADS1X4S08_AIN5            = 5,
    ADS1X4S08_AIN6            = 6,
    ADS1X4S08_AIN7            = 7,
    ADS1X4S08_AIN8            = 8,
    ADS1X4S08_AIN9            = 9,
    ADS1X4S08_AIN10           = 10,
    ADS1X4S08_AIN11           = 11,
    ADS1X4S08_AINCOM          = 12,
    ADS1X4S08_DISCONNECTED    = 13
}ads1x4s08_adc_single_input_t;

typedef enum{
    ADS1X4S08_ADC_CHANNEL_A = 0x16,
    ADS1X4S08_ADC_CHANNEL_B = 0x34,
    ADS1X4S08_ADC_CHANNEL_C = 0x25,
    ADS1X4S08_ADC_CHANNEL_D = 0x07
}ads1x4s08_adc_diff_input_t;

typedef enum{
    ADS1X4S08_DELAY_14_TMOD   = 0,
    ADS1X4S08_DELAY_25_TMOD   = 1,
    ADS1X4S08_DELAY_64_TMOD   = 2,
    ADS1X4S08_DELAY_256_TMOD  = 3,
    ADS1X4S08_DELAY_1024_TMOD = 4,
    ADS1X4S08_DELAY_2048_TMOD = 5,
    ADS1X4S08_DELAY_4096_TMOD = 6,
    ADS1X4S08_DELAY_1_TMOD = 7
}ads1x4s08_conv_delay_t;

typedef enum{
    ADS1X4S08_DISABLE = 0,
    ADS1X4S08_ENABLE = 1
}ads1x4s08_enable_t;

typedef enum{
    ADS1X4S08_GAIN_1      = 0,
    ADS1X4S08_GAIN_2      = 1,
    ADS1X4S08_GAIN_4      = 2,
    ADS1X4S08_GAIN_8      = 3,
    ADS1X4S08_GAIN_16     = 4,
    ADS1X4S08_GAIN_32     = 5,
    ADS1X4S08_GAIN_64     = 6,
    ADS1X4S08_GAIN_128    = 7
}ads1x4s08_pga_gain_t;

typedef enum{
    ADS1X4S08_FULLSCALE_5V        = ADS1X4S08_GAIN_1,
    ADS1X4S08_FULLSCALE_2V5       = ADS1X4S08_GAIN_2,
    ADS1X4S08_FULLSCALE_1V25      = ADS1X4S08_GAIN_4,
    ADS1X4S08_FULLSCALE_625mV     = ADS1X4S08_GAIN_8,
    ADS1X4S08_FULLSCALE_312mV     = ADS1X4S08_GAIN_16,
    ADS1X4S08_FULLSCALE_156mV     = ADS1X4S08_GAIN_32,
    ADS1X4S08_FULLSCALE_78mV      = ADS1X4S08_GAIN_64,
    ADS1X4S08_FULLSCALE_39mV      = ADS1X4S08_GAIN_128,
    ADS1X4S08_AUTO_CALIBRATION    = 8
}ads1x4s08_voltage_range_t;

typedef enum{
    ADS1X4S08_INT_CLK = 0,
    ADS1X4S08_EXT_CLK = 1
}ads1x4s08_clock_source_t;

typedef enum{
    ADS1X4S08_CONTINUOUS_MODE = 0,
    ADS1X4S08_SINGLE_MODE = 1
}ads1x4s08_conv_mode_t;

typedef enum{
    ADS1X4S08_DR_2_5_SPS      = 0,
    ADS1X4S08_DR_5_SPS        = 1,
    ADS1X4S08_DR_10_SPS       = 2,
    ADS1X4S08_DR_16_6_SPS     = 3,
    ADS1X4S08_DR_20_SPS       = 4,
    ADS1X4S08_DR_50_SPS       = 5,
    ADS1X4S08_DR_60_SPS       = 6,
    ADS1X4S08_DR_100_SPS      = 7,
    ADS1X4S08_DR_200_SPS      = 8,
    ADS1X4S08_DR_400_SPS      = 9,
    ADS1X4S08_DR_800_SPS      = 10,
    ADS1X4S08_DR_1000_SPS     = 11,
    ADS1X4S08_DR_2000_SPS     = 12,
    ADS1X4S08_DR_4000_SPS     = 13
}ads1x4s08_datarate_t;


typedef enum{
    ADS1X4S08_REF_P0_N0           = 0,
    ADS1X4S08_REF_P1_N1           = 1,
    ADS1X4S08_INTERNAL_REF_2V5    = 2
}ads1x4s08_ref_input_t;

typedef enum{
    ADS1X4S08_INTERNAL_REF_OFF                = 0,
    ADS1X4S08_REF_ON_BUT_POWERDOWN            = 1,
    ADS1X4S08_INTERNAL_REF_ALWAYS_ON          = 2
}ads1x4s08_ref_voltage_config_t;


typedef enum{
    ADS1X4S08_MAG_OFF     = 0,
    ADS1X4S08_MAG_10uA    = 1,
    ADS1X4S08_MAG_50uA    = 2,
    ADS1X4S08_MAG_100uA   = 3,
    ADS1X4S08_MAG_250uA   = 4,
    ADS1X4S08_MAG_500uA   = 5,
    ADS1X4S08_MAG_750uA   = 6,
    ADS1X4S08_MAG_1000uA  = 7,
    ADS1X4S08_MAG_1500uA  = 8,
    ADS1X4S08_MAG_2000uA  = 9
}ads1x4s08_idac_mag_t;

typedef enum{
    ADS1X4S08_VBIAS_DIV2  = 0,
    ADS1X4S08_VBIAS_DIV12 = 1
}ads1x4s08_vbias_select_t;

typedef enum{
    ADS1X4S08_SYS_MON_DISABLED                = 0,
    ADS1X4S08_PGA_SHORTED_TO_MID_POWER        = 1,
    ADS1X4S08_INTERNAL_TEMPERATURE_SENSOR     = 2,
    ADS1X4S08_ANALOG_SUPPLY_DIV_4             = 3,
    ADS1X4S08_DIGITAL_SUPPLY_DIV_4            = 4,
    ADS1X4S08_BURNOUT_CURRENT_0_2uA           = 5,
    ADS1X4S08_BURNOUT_CURRENT_1uA             = 6,
    ADS1X4S08_BURNOUT_CURRENT_10uA            = 7
}ads1x4s08_sys_monitor_config_t;


/* Custom OpenIndus functions */

/* Init */
void ADS1x4S08_init(gpio_num_t adc_start, gpio_num_t adc_drdy, gpio_num_t adc_reset, gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, gpio_num_t cs);

/* HAL */
void ADS1x4S08_Hal_spi_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sclk, gpio_num_t cs);

uint8_t ADS1x4S08_Hal_spi_transfer(uint8_t data);

/* Assesser */
void ADS1x4S08_get_register(ads1x4s08_reg_addr_t reg_addr, uint8_t *reg_data);

void ADS1x4S08_set_register(ads1x4s08_reg_addr_t reg_addr, uint8_t *reg_data);

void ADS1x4S08_get_default_register(void);

void ADS1x4S08_set_reference_voltage(ads1x4s08_ref_input_t ref, ads1x4s08_ref_voltage_config_t state);

void ADS1x4S08_set_datarate(ads1x4s08_datarate_t datarate);

void ADS1x4S08_set_gain(ads1x4s08_pga_gain_t gain);

bool ADS1x4S08_pga_saturation(void);

void ADS1x4S08_gain_calibration(void);

void ADS1x4S08_set_data_format(ads1x4s08_enable_t status, ads1x4s08_enable_t crc);

void ADS1x4S08_start_conversion(void);

void ADS1x4S08_get_conversion_data();

void ADS1x4S08_stop_conversion(void);

float ADS1x4S08_get_analog_voltage(void);

bool ADS1x4S08_check_data(void);

void ADS1x4S08_ISR_data_ready(void *args);

void ADS1x4S08_reset(void);

float ADS1x4S08_internal_temperature_sensor(void);

#ifdef __cplusplus
}
#endif
