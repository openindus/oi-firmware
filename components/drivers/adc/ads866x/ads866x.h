/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ads866x.h
 * @brief Drivers for ADC ADS8664 (4 Channels) and ADS8668 (8 Channels)
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "math.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ADS8664_VOLTAGE_REFERENCE (float)4.096
#define ADS866x_RESOLUTION_BITS  (uint8_t)12
#define ADS866X_MAX_CHANNELS_NB   (uint8_t)8

// COMMAND REGISTER MAP --------------------------------------------------------------------------------------------
#define ADS866X_CMD_NO_OP     0x00  // Continue operation in previous mode
#define ADS866X_CMD_STDBY     0x82  // Device is placed into standby mode
#define ADS866X_CMD_PWR_DN    0x83  // Device is powered down
#define ADS866X_CMD_RST       0x85  // Program register is reset to default
#define ADS866X_CMD_AUTO_RST  0xA0  // Auto mode enabled following a reset
#define ADS866X_CMD_MAN_CH_0  0xC0  // Channel 0 input is selected
#define ADS866X_CMD_MAN_CH_1  0xC4  // Channel 1 input is selected
#define ADS866X_CMD_MAN_CH_2  0xC8  // Channel 2 input is selected
#define ADS866X_CMD_MAN_CH_3  0xCC  // Channel 3 input is selected

// ADS8668 Only (8 Channels)
#define ADS866X_CMD_MAN_CH_4  0xD0  // Channel 4 input is selected
#define ADS866X_CMD_MAN_CH_5  0xD4  // Channel 5 input is selected
#define ADS866X_CMD_MAN_CH_6  0xD8  // Channel 6 input is selected
#define ADS866X_CMD_MAN_CH_7  0xDC  // Channel 7 input is selected

#define MAN_AUX   0xE0  // AUX channel input is selected

// PROGRAM REGISTER MAP -------------------------------------------------------------------------------------------

// AUTO SCAN SEQUENCING CONTROL
#define AUTO_SEQ_EN   0x01  // Auto Sequencing Enable: default 0xFF - bitX to enable chX
#define CH_PWR_DN     0x02  // Channel Power Down: default 0x00 - bitX to power down chX

// DEVICE FEATURES SELECTION CONTROL
#define FT_SEL        0x03  // Feature Select: default 0x00
// bit 7-6 for daisy chain ID, bit 4 for ALARM feature, bit 2-0 SDO data format bits

// RANGE SELECT REGISTERS
#define RG_CH_0       0x05   // Channel 0 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_1       0x06   // Channel 1 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_2       0x07   // Channel 2 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_3       0x08   // Channel 3 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_4       0x09   // Channel 4 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_5       0x0A   // Channel 5 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_6       0x0B   // Channel 6 Input Range: default 0x00 - bit 3-0 to select range
#define RG_CH_7       0x0C   // Channel 7 Input Range: default 0x00 - bit 3-0 to select range

// ALARM FLAG REGISTERS (Read-only)
#define ALARM_OVERVIEW          0x10 // ALARM Overview Tripped Flag
#define ALARM_CH0_TRIPPED_FLAG  0x11 // ALARM Ch 0-3 Tripped-Flag
#define ALARM_CH0_ACTIVE_FLAG   0x12 // ALARM Ch 0-3 Active-Flag
#define ALARM_CH4_TRIPPED_FLAG  0x13 // ALARM Ch 4-7 Tripped-Flag
#define ALARM_CH4_ACTIVE_FLAG   0x14 // ALARM Ch 4-7 Active-Flag

// ALARM THRESHOLD REGISTERS
#define CH0_HYST      0x15   // Ch 0 Hysteresis
#define CH0_HT_MSB    0x16   // Ch 0 High Threshold MSB
#define CH0_HT_LSB    0x17   // Ch 0 High Threshold LSB
#define CH0_LT_MSB    0x18   // Ch 0 Low Threshold MSB
#define CH0_LT_LSB    0x19   // Ch 0 Low Threshold LSB
//... CHx register address are Ch0 + 5x

// COMMAND READ BACK (Read-Only)
#define CMD_READBACK  0x3F   // Command Read Back

// SPECIFIC VALUES -------------------------------------------------------------------------------------------

//RANGE SELECTION
#define ADS866X_VOLTAGE_RANGE_0            0x00   // Input range of -/+ 2.5*Vref       (-/+ 10.24V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_1            0x01   // Input range of -/+ 1.25*Vref      (-/+  5.12V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_2            0x02   // Input range of -/+ 0.625*Vref     (-/+ 2.56V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_3            0x03   // Input range of -/+ 0.3125*Vref    (-/+ 1.28V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_4            0x0B   // Input range of -/+ 0.15625*Vref   (-/+ 0.64V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_5            0x05   // Input range of 0-2.5*Vref         (0-10.24V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_6            0x06   // Input range of 0-1.25*Vref        (0-5.12V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_7            0x07   // Input range of 0-0.625*Vref       (0-2.56V if internal reference used)
#define ADS866X_VOLTAGE_RANGE_8            0x0F   // Input range of 0-0.3125*Vref      (0-1.28V if internal reference used)

// OPERATION MODES
#define ADS866X_MODE_IDLE       (uint8_t)0
#define ADS866X_MODE_RESET      (uint8_t)1
#define ADS866X_MODE_STANDBY    (uint8_t)2
#define ADS866X_MODE_POWER_DN   (uint8_t)3
#define ADS866X_MODE_PROG       (uint8_t)4
#define ADS866X_MODE_MANUAL     (uint8_t)5
#define ADS866X_MODE_AUTO       (uint8_t)6
#define ADS866X_MODE_AUTO_RST   (uint8_t)7

typedef struct 
{
    /* SPI Config */
    spi_host_device_t spi_host;    
    int spi_freq;
    int spi_pin_cs;

    /* Pin Config */
    gpio_num_t pin_rst;
    gpio_num_t pin_alarm;

    /* ADC Config */
    uint8_t adc_channel_nb;
} ads866x_config_t;


/**
 * @brief Initialize ads866x driver
 * @param[in] config : Device configuration
 * @retval 0
 */
int ads866x_init(ads866x_config_t *config);

/**
 * @brief Read adc raw value of corresponding analog
 * @param[in] channel : Channel to read
 * @retval Measured raw value
 */
uint16_t ads866x_analog_read(uint8_t channel);


/**
 * @brief Retval analog voltage reference of ads866x
 * @retval float (value in V)
 */
float ads866x_get_voltage_reference(void);

/**
 * @brief Set analog voltage reference of ads866x
 * @param[in] ref : value to set in V
 */
void ads866x_set_voltage_reference(float ref);

/**
 * @brief Continue previous operation
 * @retval uint16_t
 */
uint16_t ads866x_noOp(void);

/**
 * @brief Reset device and registers to their default value
 * @retval uint16_t
 */
uint16_t ads866x_reset(void);

/**
 * @brief Set the device in standby mode
 * @retval uint16_t
 */
uint16_t ad866x_standby(void);

/**
 * @brief Software power down - Registers are not reset to their default values
 * @retval uint16_t
 */
uint16_t ads866x_power_down(void);

/**
 * @brief Set ADC in auto_reset mode. Enables to read a sequence channels in ascending order
 * @retval uint16_t
 */
uint16_t ads866x_auto_reset(void);

/**
 * @brief Place ADC in Manual Mode to scan from the channel in parameter
 * @param[in] channel : Number of the channel to select
 * @retval uint16_t
 */
uint16_t ads866x_manual_channel_select(uint8_t channel);

/**
 * @brief Set the AUTO_SEQ_EN register used to selec channels in auto_reset mode
 * @param[in] channels_on : vector of bits to select channels to enable
 */
void ads866x_set_channel_sequence(uint8_t channels_on);

/**
 * @brief Set the Channel Power Down register used to not include channels in auto_reset mode
 * @param[in] channels_off : vector of bits to select channels to disable
 */
void ads866x_set_channel_power_down(uint8_t channels_off);

/**
 * @brief Configure both AUTO_SEQ_EN and Channel power down registers for auto_reset mode
 * @param[in] channels : vector of bits to select channels to enable
 */
void ads866x_set_channel_SPD(uint8_t channels);

/**
 * @brief Retval the value of the AUTO_SEQ_EN Register
 */
uint8_t ads866x_get_channel_sequence(void);

/**
 * @brief Retval the value of the power down register
 */
uint8_t ads866x_get_channel_power_down(void);

/**
 * @brief Retval device id (Feature Select Register)
 */
uint8_t ads866x_get_device_id(void);

/**
 * @brief Set device id used for daisy chain (Feature Select Register)
 * @param[in] id : value of id. Must be an integer between [0;3]
 */
void ads866x_set_device_id(uint8_t id);

/**
 * @brief Retval the status of alarm (4th bit of Feature Select Register)
 * @retval 1 if alarm is enabled, 0 otherwise
 */
bool ads866x_get_alarm(void);

/**
 * @brief Configure alarm (Bit 4 of Feature Select Register)
 * @param[in] alarm : 1 (Enable), 0 (Disable)
 */
void ads866x_set_alarm(bool alarm);

/**
 * @brief Retval the SDO Format bits [0:2] contained in Feature Select Register
 * @retval uint8_t (SDO Format data bits)
 */
uint8_t ads866x_get_sdo(void);

/**
 * @brief Configure SDO Format (Feature Select Register)
 * @param[in] sdo : format
 *            Accepted values : 0, 1, 2, 3
 * Cf Datasheet for further information
 */
void ads866x_set_sdo(uint8_t sdo);

/**
 * @brief Retval the value of Feature Select Register
 * @retval uint8_t
 */
uint8_t ads866x_get_feature_select(void);


/******** Input range configuration ****************/

/**
 * @brief Retrieve the configured input voltage range for a specified channel.
 * @param[in] channel : The number of the channel.
 * @return uint8_t The voltage range of the channel.
 *
 * The possible voltage ranges are:
 * - 0000: Input range is set to ±2.5 x VREF
 * - 0001: Input range is set to ±1.25 x VREF
 * - 0010: Input range is set to ±0.625 x VREF
 * - 0011: Input range is set to ±0.3125 x VREF
 * - 1011: Input range is set to ±0.15625 x VREF
 * - 0101: Input range is set to 0 to 2.5 x VREF
 * - 0110: Input range is set to 0 to 1.25 x VREF
 * - 0111: Input range is set to 0 to 0.625 x VREF
 * - 1111: Input range is set to 0 to 0.3125 x VREF
 */
uint8_t ads866x_get_channel_voltage_range(uint8_t channel);

/**
 * @brief Configure input range for the specified channel
 * @param[in] channel : Channel to configure
 * @param[in] range : Voltage range input for the channel
 */
void ads866x_set_channel_voltage_range(uint8_t channel, uint8_t range);

/**
 * @brief Configure voltage input range for all channels
 */
void ads866x_set_global_voltage_range(uint8_t range);


/************* Alarm Active and Tripped flags ************/

/**
 * @brief Retval the logical OR of high and low tripped alarm flags for all eight channels
 * Bit n = Tripped Alarm Flag for Channel n
 */
uint8_t ads866x_get_alarm_overview();

/**
 * @brief Retval alarm tripped flag Low and High for register Ch0-Ch3
 * @retval uint8_t
 */
uint8_t ads866x_get_first_tripped_flag();

/**
 * @brief Retval alarm tripped flag Low and High for register Ch4-Ch7
 * @retval uint8_t
 */
uint8_t ads866x_get_second_tripped_flag();

/**
 * @brief Retval alarm tripped flag Low and High for register Ch0-Ch7
 * @retval uint16_t
 */
uint16_t ads866x_get_all_tripped_flags();

/**
 * @brief Retval alarm active flag Low and High for register Ch0-Ch3
 * @retval uint8_t
 */
uint8_t ads866x_get_first_active_flag();

/**
 * @brief Retval alarm active flag Low and High for register Ch4-Ch7
 * @retval uint8_t
 */
uint8_t ads866x_get_second_active_flag();

/**
 * @brief Retval alarm active flag Low and High for register Ch0-Ch7
 * @retval uint16_t
 */
uint16_t ads866x_get_all_active_flags();

/**
 * @brief Retval the configured hysteresis for corresponding channel
 * @param[in] channel : Corresponding channel
 * @retval uint8_t : Hysteresis is coded on 4 LSB bits
 */
uint8_t ads866x_get_channel_hysteresis(uint8_t channel);

/**
 * @brief Retval the configured low threshold for corresponding channel
 * @param[in] channel : Corresponding channel
 * @retval uint8_t : Low Threshold is coded on 12 LSB bits
 */
uint16_t ads866x_get_channel_low_threshold(uint8_t channel);

/**
 * @brief Retval the configured high threshold for corresponding channel
 * @param[in] channel : Corresponding channel
 * @retval uint8_t : High Threshold is coded on 12 LSB bits
 */
uint16_t ads866x_get_channel_high_threshold(uint8_t channel);

/**
 * @brief Set channel hysteresis
 * @param[in] channel : Corresponding channel
 * @param[in] hysteresis : value of hysteresis (4bits)
 */
void ads866x_set_channel_hysteresis(uint8_t channel, uint8_t hysteresis);

/**
 * @brief Set channel low threshold
 * @param[in] channel : channel of the channel
 * @param[in] hysteresis : value of low threshold (12bits)
 */
void ads866x_set_channel_low_threshold(uint8_t channel, uint16_t threshold);

/**
 * @brief Set channel high threshold
 * @param[in] channel : channel of the channel
 * @param[in] hysteresis : value of high threshold (12bits)
 */
void ads866x_set_channel_high_threshold(uint8_t channel, uint16_t threshold);

/**
 * @brief Read the device mode (Previous command)
 * @retval uint8_t
 */
uint8_t ads866x_get_command_readback(void);


/************* SPI Read/Write Functions *****************/

/**
 * @brief Spi write register function
 * @param[in] reg : Register to write
 * @param[in] value : Value to be writted 
 * @retval esp_err_t 
 */
esp_err_t ads866x_spi_write_register(uint8_t reg, uint8_t value);

/**
 * @brief Spi read register function
 * @param[in] reg : Register to be read
 * @retval Value of the specified register
 */
uint8_t ads866x_spi_read_program_register(uint8_t reg);

/**
 * @brief Spi write command register
 * @param[in] reg : Register to write
 * @retval Value of the register
 */
uint16_t ads866x_spi_write_command_register(uint8_t reg);


/************* Convertions Functions *****************/

/**
 * @brief Convert a raw value to volt according to configured input voltage range
 * @param[in] raw : raw value to convert
 * @retval Converted value in volt
 */
float ads866x_convert_raw_2_volt(uint16_t raw, uint8_t range);

/**
 * @brief Convert a voltage value to raw value according to configured input voltage range
 * @param[in] voltage : voltage value to convert
 * @retval Converted value in raw format (12 bits adc resolution)
 */
uint16_t ads866x_convert_volt_2_raw(float voltage, uint8_t range);

#ifdef __cplusplus
}
#endif