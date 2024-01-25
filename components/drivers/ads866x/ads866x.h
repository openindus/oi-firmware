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

#define ADS8664_ANALOG_REFERENCE (float)4.096
#define ADS866x_RESOLUTION_BITS  (uint8_t)12
#define ADS866X_MAX_ANALOGS_NB   (uint8_t)8
#define ADS866x_CURRENT_MES_RES_VALUE (float)100.0
// COMMAND REGISTER MAP --------------------------------------------------------------------------------------------

#define ADS866X_CMD_NO_OP     0x00  // Continue operation in previous mode
#define ADS866X_CMD_STDBY     0x82  // Device is placed into standby mode
#define ADS866X_CMD_PWR_DN    0x83  // Device is powered down
#define ADS866X_CMD_RST       0x85  // Program register is reset to default
#define ADS866X_CMD_AUTO_RST  0xA0  // Auto mode enabled following a reset
#define ADS866X_CMD_MAN_Ch_0  0xC0  // Channel 0 input is selected
#define ADS866X_CMD_MAN_Ch_1  0xC4  // Channel 1 input is selected
#define ADS866X_CMD_MAN_Ch_2  0xC8  // Channel 2 input is selected
#define ADS866X_CMD_MAN_Ch_3  0xCC  // Channel 3 input is selected

// ADS8668 Only (8 Channels)
#define ADS866X_CMD_MAN_Ch_4  0xD0  // Channel 4 input is selected
#define ADS866X_CMD_MAN_Ch_5  0xD4  // Channel 5 input is selected
#define ADS866X_CMD_MAN_Ch_6  0xD8  // Channel 6 input is selected
#define ADS866X_CMD_MAN_Ch_7  0xDC  // Channel 7 input is selected

#define MAN_AUX   0xE0  // AUX channel input is selected

// PROGRAM REGISTER MAP -------------------------------------------------------------------------------------------

// AUTO SCAN SEQUENCING CONTROL
#define AUTO_SEQ_EN   0x01  // Auto Sequencing Enable: default 0xFF - bitX to enable chX
#define CH_PWR_DN     0x02  // Channel Power Down: default 0x00 - bitX to power down chX

// DEVICE FEATURES SELECTION CONTROL
#define FT_SEL        0x03  // Feature Select: default 0x00
// bit 7-6 for daisy chain ID, bit 4 for ALARM feature, bit 2-0 SDO data format bits

// RANGE SELECT REGISTERS
#define RG_Ch_0       0x05   // Channel 0 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_1       0x06   // Channel 1 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_2       0x07   // Channel 2 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_3       0x08   // Channel 3 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_4       0x09   // Channel 4 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_5       0x0A   // Channel 5 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_6       0x0B   // Channel 6 Input Range: default 0x00 - bit 3-0 to select range
#define RG_Ch_7       0x0C   // Channel 7 Input Range: default 0x00 - bit 3-0 to select range

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
#define ADS866X_R0            0x00   // Input range to -2.5/+2.5         Vref   +/- 10.24V
#define ADS866X_R1            0x01   // Input range to -1.25/+1.25       Vref   +/-  5.12V
#define ADS866X_R2            0x02   // Input range to -0.625/+0.625     Vref   +/-  2.56V
#define ADS866X_R3            0x03   // Input range to -0.3125/+0.3125   Vref   +/-  1.28V
#define ADS866X_R4            0x0B   // Input range to -0.15625/+0.15625 Vref   +/-  0.64V
#define ADS866X_R5            0x05   // Input range to +2.5    Vref   10.24V
#define ADS866X_R6            0x06   // Input range to +1.25   Vref    5.12V
#define ADS866X_R7            0x07   // Input range to +0.625  Vref    2.56V
#define ADS866X_R8            0x0F   // Input range to +0.3125 Vref    1.28V

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
    /* SPI Conf */
    spi_host_device_t spi_host;    
    int spi_freq;
    int spi_pin_cs;

    /* Pin Config */
    gpio_num_t pin_rst;
    gpio_num_t pin_mode[ADS866X_MAX_ANALOGS_NB];

    /* ADC Config */
    uint8_t adc_analogs_nb;
    uint8_t adc_res;
    uint8_t adc_mode[ADS866X_MAX_ANALOGS_NB];

} Ads866x_DeviceConfig_t;

typedef enum
{
    ADS866X_CURRENT_MODE = 0,
    ADS866X_VOLTAGE_MODE = 1
} Ads866x_AdcMode_t;

typedef enum
{
    ADS866x_ADC_RES_10BITS = 10,
    ADS866x_ADC_RES_12BITS = 12
} Ads866x_AdcResolutionBits_t;

typedef enum
{
    ADS866x_UNITS_RAW,
    ADS866x_UNITS_MILLIVOLTS,
    ADS866x_UNITS_MILLIAMPS,
    ADS866x_UNITS_VOLTS,
    ADS866x_UNITS_AMPS,
} Ads866x_Units_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Configure Ads866x device
 * @param config SPI and GPIOs configuration
 * @retval void
 */
void Ads866x_DeviceConfig(Ads866x_DeviceConfig_t *config);

/**
 * @brief Initializes ads866x driver
 * @retval None
 */
uint8_t Ads866x_Init(void);

/**
 * @brief Initializes gpios used by Ads866x
 * @retval None
 */
void Ads866x_GpioInit(void);

/**
 * @brief Initializes ads866x spi device
 * @retval None
 */
void Ads8866x_SpiInit(void);

/**
 * @brief Read adc raw value of corresponding analog
 * @param[in] analogNum : index of the analog
 * @retval Measured raw value
 */
uint32_t Ads866x_AnalogRead(uint32_t analogNum);

/**
 * @brief Read adc value of corresponding analog in specified units
 * @param[in] analogNum : index of the analog
 * @param[in] units : desired unit of the result
 * @retval float Measured value
 */
float Ads866x_AnalogReadUnits(uint32_t analogNum, Ads866x_Units_t units);

/**
 * @brief Configure analog in voltage or current mode
 * @param[in] analogNum : index of the analog to configure
 * @param[in] mode : Current mode (0) or Voltage mode (1)
 * @retval None
 */
void Ads866x_setAdcMode(uint32_t analogNum, Ads866x_AdcMode_t mode);

/**
 * @brief Set Adc resolution
 * @param[in] analogRes : adc resolution (10, 12) bits
 * @retval None
 */
void Ads866x_setAdcResolution(Ads866x_AdcResolutionBits_t analogRes);

/**
 * @brief Get Adc resolution
 * @retval Current adc resolution 
 */
uint8_t Ads866x_getAdcResolution(void);

/**
 * @brief retvals analog voltage reference of Ads866x
 * @retval float
 */
float Ads866x_getAnalogReference(void);

/**
 * @brief Sets analog voltage reference of Ads866x
 * @param[in] analogReference : value to set
 */
void Ads866x_setAnalogReference(float analogReference);

/**
 * @brief Continue previous operation
 * @retval uint16_t
 */
uint16_t Ads866x_NoOp(void);

/**
 * @brief Reset device and registers to their default value
 * @retval uint16_t
 */
uint16_t Ads866x_Reset(void);

/**
 * @brief Set the devince in standby mode
 * @retval uint16_t
 */
uint16_t Ad866x_Standby(void);

/**
 * @brief Software power down - Registers are not reset to their default values
 * @retval uint16_t
 */
uint16_t Ads866x_PowerDown(void);

/**
 * @brief Set ADC in auto_rst mode. Enables to read a sequence channels in ascending order
 * @retval uint16_t
 */
uint16_t Ads866x_AutoRst(void);

/**
 * @brief Place ADC in Manual Mode to scan from the channel in parameter
 * @param[in] channelNum : Number of the channel to select
 * @retval uint16_t
 */
uint16_t Ads866x_ManualChannelSelect(uint8_t channelNum);

/**
 * @brief Set the AUTO_SEQ_EN register used to selec channels in Auto_RSt mode
 * @param[in] channels_on : vector of bits to select channels to enable
 */
void Ads866x_setChannelSequence(uint8_t channels_on);

/**
 * @brief Set the Channel Power Down register used to not include channels in Auto_Rst mode
 * @param[in] channels_on : vector of bits to select channels to disable
 */
void Ads866x_setChannelPowerDown(uint8_t channels_off);

/**
 * @brief Configure both AUTO_SEQ_EN and Channel power down registers for Auto_RST mode
 * @param[in] channels_on : vector of bits to select channels to enable
 */
void Ads866x_setChannelSPD(uint8_t channels);

/**
 * @brief retvals the value of the AUTO_SEQ_EN Register
 */
uint8_t Ads866x_getChannelSequence(void);

/**
 * @brief retvals the value of the power down register
 */
uint8_t Ads866x_getChannelPowerDown(void);

/**
 * @brief retvals device id (Feature Select Register)
 */
uint8_t Ads866x_GetDeviceId(void);

/**
 * @brief Set device id used for daisy chain (Feature Select Register)
 * @param[in] id : value of id. Must be an integer between [0;3]
 */
void Ads866x_SetDeviceId(uint16_t id);

/**
 * @brief retvals the status of alarm (4th bit of Feature Select Register)
 * @retval 1 if alarm is enabled, 0 otherwise
 */
bool Ads866x_getAlarm(void);

/**
 * @brief Configure alarm (Bit 4 of Feature Select Register)
 * @param[in] alarm : 1 (Enable), 0 (Disable)
 */
void Ads866x_setAlarm(bool alarm);

/**
 * @brief retvals the SDO Format bits [0:2] contained in Feature Select Register
 * @retval uint8_t (SDO Format data bits)
 */
uint8_t Ads866x_getSdo(void);

/**
 * @brief Configure SDO Format (Feature Select Register)
 * @param[in] sdo format
 *            Accepted values : 0, 1, 2, 3
 * Cf Datasheet for further information
 */
void Ads866x_setSdo(uint8_t sdo);

/**
 * @brief retvals the value of Feature Select Register
 * @retvals uint8_t
 */
uint8_t Ads866x_getFeatureSelect(void);

/******** Input range configuration ****************/

/**
 * @brief retvals the configured input range for channel in parameter
 * @retvals uint8_t
 * 
 * 0000 = Input range is set to ±2.5 x VREF
 * 0001 = Input range is set to ±1.25 x VREF
 * 0010 = Input range is set to ±0.625 x VREF
 * 0011 = Input range is set to ±0.3125 x VREF
 * 1011 = Input range is set to ±0.15625 x VREF
 * 0101 = Input range is set to 0 to 2.5 x VREF
 * 0110 = Input range is set to 0 to 1.25 x VREF
 * 0111 = Input range is set to 0 to 0.625 x VREF
 * 1111 = Input range is set to 0 to 0.3125 x VREF
 */
uint8_t Ads866x_getChannelRange(uint8_t channel);

/**
 * @brief Configure input range for the specified channel
 * @param[in] channel : Channel to configure
 * @param[in] range : Range input for the channel
 */
void Ads866x_setChannelRange(uint8_t channel, uint8_t range);

/**
 * @brief Configure input range for all channels
 */
void Ads866x_setGlobalRange(uint8_t range);

/************* Alarm Active and Tripped flags ************/

/**
 * @brief retvals the logical OR of high and low tripped alarm flags for all eight channels
 * Bit n = Tripped Alarm Flag for Channel n
 */
uint8_t Ads866x_getAlarmOverview();

/**
 * @brief retvals alarm tripped flag Low and High for register Ch0-Ch3
 * @retvals uint8_t
 */
uint8_t Ads866x_getFirstTrippedFlag();

/**
 * @brief retvals alarm tripped flag Low and High for register Ch4-Ch7
 * @retvals uint8_t
 */
uint8_t Ads866x_getSecondTrippedFlag();

/**
 * @brief retvals alarm tripped flag Low and High for register Ch0-Ch7
 * @retvals uint16_t
 */
uint16_t Ads866x_getTrippedFlags();

/**
 * @brief retvals alarm active flag Low and High for register Ch0-Ch3
 * @retvals uint8_t
 */
uint8_t Ads866x_getFirstActiveFlag();

/**
 * @brief retvals alarm active flag Low and High for register Ch0-Ch7
 * @retvals uint8_t
 */
uint8_t Ads866x_getSecondActiveFlag();

/**
 * @brief retvals alarm active flag Low and High for register Ch0-Ch7
 * @retvals uint16_t
 */
uint16_t Ads866x_getActiveFlags();

/**
 * @brief retvals the configured hysteresis for corresponding channel
 * @param[in] channel : num of the channel
 * @retvals uint8_t : Hysteresis is coded on 4 LSB bits
 */
uint8_t Ads866x_getChannelHysteresis(uint8_t channel);

/**
 * @brief retvals the configured low threshold for corresponding channel
 * @param[in] channel : num of the channel
 * @retvals uint8_t : Low Threshold is coded on 12 LSB bits
 */
uint16_t Ads866x_getChanneLowThreshold(uint8_t channel);

/**
 * @brief retvals the configured high threshold for corresponding channel
 * @param[in] channel : num of the channel
 * @retvals uint8_t : High Threshold is coded on 12 LSB bits
 */
uint16_t Ads866x_getChanneHighThreshold(uint8_t channel);

/**
 * @brief Set channel hysteresis
 * @param[in] channel : num of the channel
 * @param[in] hysteresis : value of hysteresis (4bits)
 */
void Ads866x_setChannelHysteresis(uint8_t channel, uint8_t hysteresis);

/**
 * @brief Set channel low threshold
 * @param[in] channel : num of the channel
 * @param[in] hysteresis : value of low threshold (12bits)
 */
void Ads866x_setChannelLowThreshold(uint8_t channel, uint16_t threshold);

/**
 * @brief Set channel high threshold
 * @param[in] channel : num of the channel
 * @param[in] hysteresis : value of high threshold (12bits)
 */
void Ads866x_setChannelHighThreshold(uint8_t channel, uint16_t threshold);

/**
 * @brief Read the device mode (Previous command)
 * @retvals uint8_t
 */
uint8_t Ads866x_getCommandReadBack(void);

/************* SPI Read/Write Functions *****************/
/**
 * @brief Spi write register function
 * @param reg : Register to write
 * @param value : Value to be writted 
 * @retval esp_err_t 
 */
esp_err_t Ads866x_SpiWriteRegister(uint8_t reg, uint8_t value);

/**
 * @brief Spi read register function
 * @param reg : Register to be read
 * @retval Value of the specified register
 */
uint8_t Ads866x_SpiReadRegister(uint8_t reg);

/**
 * @brief Spi write command register
 * @param reg : Register to write
 * @retval Value of the register
 */
uint16_t Ads866x_SpiWriteCmdRegister(uint8_t reg);

/**
 * @brief Convert a raw value to volt according to configured input range
 * @param[in] rawValue : raw value to convert
 * @param[in] inputRange : inputRange configuration used for conversion
 * @retval Converted value in volt
 */
float Ads866x_ConvertRaw2Volt(uint16_t rawValue, uint8_t inputRange);

/**
 * @brief Convert a voltage value to raw value according to configured input range
 * @param[in] voltageValue : voltage value to convert
 * @param[in] inputRange : inputRange configuration used for conversion
 * @retval Converted value in raw format (12 bits adc resolution)
 */
uint16_t Ads866x_ConvertVolt2Raw(float voltageValue, uint8_t inputRange);

#ifdef __cplusplus
}
#endif