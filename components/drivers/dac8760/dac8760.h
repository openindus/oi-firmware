/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file dac8760.h
 * @brief Drivers for DAC8760
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DAC8760_MAX_DEVICES     (uint8_t)2
#define DAC8760_MODE_UNDEFINED  (uint8_t) 0xFF

//Address Byte
#define DAC8760_ADDR_NOP        0x00 //No operation (NOP)
#define DAC8760_ADDR_DATA       0x01 //Write DAC data register
#define DAC8760_ADDR_READ       0x02 //Register read
#define DAC8760_ADDR_CTRL       0x55 //Write control register
#define DAC8760_ADDR_RESET      0x56 //Write reset register
#define DAC8760_ADDR_CONFIG     0x57 //Write configuration register
#define DAC8760_ADDR_GAIN       0x58 //Write DAC gain calibration register
#define DAC8760_ADDR_ZERO       0x59 //Write DAC zero calibration register
#define DAC8760_ADDR_WDT        0x95 //Watchdog timer reset

//CTRL Bits
#define DAC8760_CTRL_CLRSEL     15 //VOUT clear value: 0 = VOUT is 0V, 1 = VOUT is midscale in unipolar output and negative-full-scale in bipolar output
#define DAC8760_CTRL_OVR        14 //Setting the bit increases the voltage output range by 10%.
#define DAC8760_CTRL_REXT       13 //External current setting resistor enable.
#define DAC8760_CTRL_OUTEN      12 //0 = output is disabled, 1 = output is determined by RANGE bits
#define DAC8760_CTRL_SRCLK       8 //Slew rate clock control. Ignored when bit SREN = 0
#define DAC8760_CTRL_SRSTEP      5 //Slew rate step size control. Ignored when bit SREN = 0
#define DAC8760_CTRL_SREN        4 //Slew Rate Enable.
#define DAC8760_CTRL_DCEN        3 //Daisy-chain enable.
#define DAC8760_CTRL_RANGE       0 //Output range bits.

typedef struct 
{
    uint8_t mode;
    int8_t min;
    int8_t max;
    uint32_t res;
} Dac8760_Configuration_t;

typedef struct 
{
    /* SPI Conf */
    spi_host_device_t spi_host;    
    int spi_freq;
    int spi_pin_cs[DAC8760_MAX_DEVICES];

    /* DAC Config */
    uint8_t nb_devices;
    Dac8760_Configuration_t conf[DAC8760_MAX_DEVICES];
} Dac8760_DeviceConfig_t;


typedef enum
{
    DAC8760_VOLTAGE_RANGE_0_5_V,
    DAC8760_VOLTAGE_RANGE_0_10_V,
    DAC8760_VOLTAGE_RANGE_N5_5_V,
    DAC8760_VOLTAGE_RANGE_N10_10_V,
    DAC8760_VOLTAGE_RANGE_INVALID = 0xFF,
} Dac8760_VoltageMode_t;

typedef enum
{
    DAC8760_CURRENT_RANGE_4_20_MA,
    DAC8760_CURRENT_RANGE_0_20_MA,
    DAC8760_CURRENT_RANGE_0_24_MA,
    DAC8760_CURRENT_RANGE_INVALID = 0xFF,
} Dac8760_CurrentMode_t;

typedef enum
{
    DAC8760_UNITS_RAW,
    DAC8760_UNITS_MILLIVOLTS,
    DAC8760_UNITS_MILLIAMPS,
    DAC8760_UNITS_VOLTS,
    DAC8760_UNITS_AMPS,
} Dac8760_Units_t;


/**
 * @brief Configure Dac8760 device
 * @param config SPI and GPIOs configuration
 * @retval void
 */
void Dac8760_DeviceConfig(Dac8760_DeviceConfig_t *config);

/**
 * @brief Initializes Dac8760 driver
 * @retval None
 */
void Dac8760_Init(void);

/**
 * @brief Initializes Dac8760 spi bus device
 * @retval None
 */
void Dac8760_SpiInit(void);


/**
 * @brief Set the range when dac is used as a voltage output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @param[in] mode : Range of the voltage output
 * @retval None
 */
void Dac8760_setVoltageMode(uint8_t deviceId, Dac8760_VoltageMode_t mode);

/**
 * @brief Retrieve the voltage mode of the corresponding output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @retval Dac8760_VoltageMode_t : Voltage configuration range of the selected output
 */
Dac8760_VoltageMode_t Dac8760_getVoltageMode(uint8_t deviceId);

/**
 * @brief Write voltage output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @param[in] value : Value expressed in the specified units
 * @param[in] units : Specify the units (default = volts)
 * @retval None
 */
void Dac8760_VoltageWrite(uint8_t deviceId, float value, Dac8760_Units_t units);

/**
 * @brief Set the range when dac is used as a current output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @param[in] mode : Range of the current output
 * @retval None
 */
void Dac8760_setCurrentMode(uint8_t deviceId, Dac8760_CurrentMode_t mode);

/**
 * @brief Retrieve the current mode of the corresponding output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @retval Dac8760_CurrentMode_t : Current configuration range of the selected output
 */
Dac8760_CurrentMode_t Dac8760_getCurrentMode(uint8_t deviceId);

/**
 * @brief Write current output
 * @param[in] deviceId : (from 0 to DAC8760_MAX_DEVICES - 1)
 * @param[in] value : Value expressed in the specified units
 * @param[in] units : Specify the units (default = milliamps)
 * @retval None
 */
void Dac8760_CurrentWrite(uint8_t deviceId, float value, Dac8760_Units_t units);

/**
 * @brief Dac8760 Spi write register function
 * @param reg : Register to write
 * @param value : Value to be writted 
 * @retval esp_err_t 
 */
esp_err_t Dac8760_SpiWriteRegister(uint8_t deviceId, uint8_t reg, uint16_t data);

#ifdef __cplusplus
}
#endif