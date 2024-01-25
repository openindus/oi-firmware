/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file CorePinout.h
 * @brief Functions for core module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once


#if defined(CONFIG_CORE)

/* I2C pins */
#define CORE_PIN_I2C_SDA              GPIO_NUM_32
#define CORE_PIN_I2C_SCL              GPIO_NUM_33

/* SPI pins */
#define CORE_PIN_SPI_MISO             GPIO_NUM_12
#define CORE_PIN_SPI_MOSI             GPIO_NUM_13
#define CORE_PIN_SPI_SCK              GPIO_NUM_14

/* RS EXT pins */
#define CORE_PIN_RS_INTERRUPT         GPIO_NUM_34
#define CORE_PIN_RS_SPI_CS            GPIO_NUM_19
#define CORE_IOEX_PIN_RS_RESET        IOEX_NUM_21

/* CAN EXT pins */
#define CORE_PIN_CAN_INTERRUPT        GPIO_NUM_23
#define CORE_PIN_CAN_SPI_CS           GPIO_NUM_27
#define CORE_IOEX_PIN_CAN_RESET       IOEX_NUM_20

/* USB Host pins */
#define CORE_PIN_USB_HOST_SPI_CS      GPIO_NUM_18
#define CORE_IOEX_PIN_USB_HOST_RESET  IOEX_NUM_23

/* Ethernet pins */
#define CORE_PIN_ETHERNET_SPI_CS      GPIO_NUM_25
#define CORE_IOEX_PIN_ETHERNET_RESET  IOEX_NUM_22

/* USB Host pins */
#define CORE_PIN_SD_SPI_CS            GPIO_NUM_26

/* RTC pins */
#define CORE_PIN_RTC_INTERRUPT        IOEX_NUM_25

/* Module ext pins */
#define CORE_PIN_MODULE_EXT_IO15      GPIO_NUM_15
#define CORE_PIN_MODULE_EXT_IO2       GPIO_NUM_12

/* DIN Interrupt */
#if CONFIG_IDF_TARGET_ESP32
#define CORE_PIN_DIGITAL_INTERRUPT       GPIO_NUM_22
#else // Dummy value if esp32s2
#define CORE_PIN_DIGITAL_INTERRUPT       (gpio_num_t) 0
#endif

/* AIN */
#if CONFIG_IDF_TARGET_ESP32
#define CORE_CHANNEL_AIN_1            ADC1_GPIO36_CHANNEL
#define CORE_CHANNEL_AIN_2            ADC1_GPIO39_CHANNEL
#else // Dummy value if esp32s2
#define CORE_CHANNEL_AIN_1            (adc1_channel_t) 0
#define CORE_CHANNEL_AIN_2            (adc1_channel_t) 0
#endif

/* DOUT */
#define CORE_IOEX_PIN_DOUT_1           IOEX_NUM_0
#define CORE_IOEX_PIN_DOUT_2           IOEX_NUM_1
#define CORE_IOEX_PIN_DOUT_3           IOEX_NUM_2
#define CORE_IOEX_PIN_DOUT_4           IOEX_NUM_3

/* DOUT Sensor */
#define CORE_IOEX_PIN_DOUT_CURRENT_1    IOEX_NUM_4
#define CORE_IOEX_PIN_DOUT_CURRENT_2    IOEX_NUM_5
#define CORE_IOEX_PIN_DOUT_CURRENT_3    IOEX_NUM_6
#define CORE_IOEX_PIN_DOUT_CURRENT_4    IOEX_NUM_7

/* DIN */
#define CORE_IOEX_PIN_DIN_1           IOEX_NUM_10
#define CORE_IOEX_PIN_DIN_2           IOEX_NUM_11
#define CORE_IOEX_PIN_DIN_3           IOEX_NUM_12
#define CORE_IOEX_PIN_DIN_4           IOEX_NUM_13

/* MODULE EXT */
#define CORE_IOEX_PIN_MODULE_EXT_0    IOEX_NUM_14
#define CORE_IOEX_PIN_MODULE_EXT_1    IOEX_NUM_15
#define CORE_IOEX_PIN_MODULE_EXT_2    IOEX_NUM_16
#define CORE_IOEX_PIN_MODULE_EXT_3    IOEX_NUM_17

/* RTC */
#define CORE_IOEX_PIN_RTC_INT         IOEX_NUM_25
#define CORE_RTC_INTERRUPT_PRIORITY   10

/* Command mosfet alim ext */
#define CORE_IOEX_PIN_CMD_MOSFET      IOEX_NUM_24

/* Command mosfet alim ext */
#define CORE_IOEX_PIN_ALIM_EXT        IOEX_NUM_27

/* Module configuration */
#define CORE_I2C_PORT_NUM             I2C_NUM_0
#define CORE_SPI_HOST                 SPI2_HOST

#define CORE_I2C_IOEXPANDER_ADDRESS   0x23
#define CORE_I2C_RTC_ADDRESS          0xD0
#define CORE_DEFAULT_I2C_SPEED        100000

#define CORE_DIGITAL_INTERRUPT_PRIORITY  20U

#define CORE_ADC_NO_OF_SAMPLES        64U
#define CORE_ADC_REDUCTION_FACTOR     9.432f

#endif
