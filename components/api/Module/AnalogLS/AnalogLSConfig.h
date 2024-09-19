/**
 * @file AnalogLSConfig.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_ANALOG_LS)

/* SPI */
#define ANALOG_LS_SPI_HOST              SPI2_HOST
#define ANALOG_LS_SPI_PIN_MISO          GPIO_NUM_18
#define ANALOG_LS_SPI_PIN_MOSI          GPIO_NUM_37
#define ANALOG_LS_SPI_PIN_SCLK          GPIO_NUM_15

/* ADC device */
#define ANALOG_LS_ADC_SPI_FREQ          SPI_MASTER_FREQ_10M
#define ANALOG_LS_ADC_PIN_CS            GPIO_NUM_17
#define ANALOG_LS_ADC_PIN_START_SYNC    GPIO_NUM_47
#define ANALOG_LS_ADC_PIN_DRDY          GPIO_NUM_33
#define ANALOG_LS_ADC_PIN_RESET         GPIO_NUM_16

/* IO Mux */
#define ANALOG_LS_MUX_PIN_HS1_A0        GPIO_NUM_7
#define ANALOG_LS_MUX_PIN_HS1_A1        GPIO_NUM_8
#define ANALOG_LS_MUX_PIN_HS1_A2        GPIO_NUM_9
#define ANALOG_LS_MUX_PIN_HS2_A0        GPIO_NUM_10
#define ANALOG_LS_MUX_PIN_HS2_A1        GPIO_NUM_11
#define ANALOG_LS_MUX_PIN_HS2_A2        GPIO_NUM_12
#define ANALOG_LS_MUX_PIN_LS1_A0        GPIO_NUM_13
#define ANALOG_LS_MUX_PIN_LS1_A1        GPIO_NUM_14
#define ANALOG_LS_MUX_PIN_LS1_A2        GPIO_NUM_19
#define ANALOG_LS_MUX_PIN_LS2_A0        GPIO_NUM_20
#define ANALOG_LS_MUX_PIN_LS2_A1        GPIO_NUM_21
#define ANALOG_LS_MUX_PIN_LS2_A2        GPIO_NUM_26

/* Sensor fault */
#define ANALOG_LS_SENSOR_FAULT_PIN_5V   GPIO_NUM_1
#define ANALOG_LS_SENSOR_FAULT_PIN_3V3  GPIO_NUM_2

/* I2C */
#define ANALOG_LS_I2C_PORT_NUM          I2C_NUM_0
#define ANALOG_LS_I2C_PIN_SCL           GPIO_NUM_6
#define ANALOG_LS_I2C_PIN_SDA           GPIO_NUM_5

/* Thermometer */
#define ANALOG_LS_THERM_PIN_OS_INT      GPIO_NUM_4
#define ANALOG_LS_THERM_I2C_ADDR        0x90

/* Digipot */
#define ANALOG_LS_DIGIPOT_I2C_ADDR      0x50

#endif