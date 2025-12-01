/**
 * @file MixedPinout.h
 * @brief Mixed Module Pinout
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_OI_MIXED)

/* DIN */
#define MIXED_GPIO_DIN_1            GPIO_NUM_47
#define MIXED_GPIO_DIN_2            GPIO_NUM_21
#define MIXED_GPIO_DIN_3            GPIO_NUM_19
#define MIXED_GPIO_DIN_4            GPIO_NUM_33

/* DOUT */
#define MIXED_GPIO_DOUT_1           GPIO_NUM_10
#define MIXED_GPIO_DOUT_2           GPIO_NUM_13
#define MIXED_GPIO_DOUT_3           GPIO_NUM_5
#define MIXED_GPIO_DOUT_4           GPIO_NUM_8
#define MIXED_ADC_CHANNEL_DOUT_1    ADC_CHANNEL_8
#define MIXED_ADC_CHANNEL_DOUT_2    ADC_CHANNEL_0
#define MIXED_ADC_CHANNEL_DOUT_3    ADC_CHANNEL_0
#define MIXED_ADC_CHANNEL_DOUT_4    ADC_CHANNEL_6
#define MIXED_ADC_UNIT_DOUT_1       ADC_UNIT_1
#define MIXED_ADC_UNIT_DOUT_2       ADC_UNIT_2
#define MIXED_ADC_UNIT_DOUT_3       ADC_UNIT_1
#define MIXED_ADC_UNIT_DOUT_4       ADC_UNIT_1

/* EANA */
#define MIXED_PIN_EANA_1            AIN_NUM_2
#define MIXED_PIN_EANA_2            AIN_NUM_1
#define MIXED_PIN_EANA_3            AIN_NUM_3
#define MIXED_PIN_EANA_4            AIN_NUM_4

/* EANA_Commands */
#define MIXED_PIN_EANA_CMD_1        GPIO_NUM_16
#define MIXED_PIN_EANA_CMD_2        GPIO_NUM_18
#define MIXED_PIN_EANA_CMD_3        GPIO_NUM_20
#define MIXED_PIN_EANA_CMD_4        GPIO_NUM_26

/* SPI */
#define MIXED_SPI_HOST              SPI2_HOST
#define MIXED_SPI_FREQ              SPI_MASTER_FREQ_8M
#define MIXED_SPI_PIN_MISO          GPIO_NUM_36
#define MIXED_SPI_PIN_MOSI          GPIO_NUM_48
#define MIXED_SPI_PIN_SCK           GPIO_NUM_37

/* ADC */
#define MIXED_ADC_NB                (4)
#define MIXED_ADC_PIN_RST           GPIO_NUM_35
#define MIXED_ADC_PIN_CS            GPIO_NUM_34
#define MIXED_ADC_PIN_ALARM         GPIO_NUM_2

/* DAC */
#define MIXED_DAC_NB                (2)
#define MIXED_DAC_PIN_SYNC_1        GPIO_NUM_4 // CS1
#define MIXED_DAC_PIN_SYNC_2        GPIO_NUM_17 // CS2
#define MIXED_DAC_PIN_LDAC_1        GPIO_NUM_6
#define MIXED_DAC_PIN_LDAC_2        GPIO_NUM_15

#endif