/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MixedPinout.h
 * @brief Functions for mixed module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_MIXED)

/* DIN */
#define MIXED_PIN_DIN_1                 GPIO_NUM_47
#define MIXED_PIN_DIN_2                 GPIO_NUM_21
#define MIXED_PIN_DIN_3                 GPIO_NUM_19
#define MIXED_PIN_DIN_4                 GPIO_NUM_33

/* DOUT */
#define MIXED_PIN_DOUT_1                GPIO_NUM_10
#define MIXED_PIN_DOUT_2                GPIO_NUM_13
#define MIXED_PIN_DOUT_3                GPIO_NUM_5
#define MIXED_PIN_DOUT_4                GPIO_NUM_8

/* DOUT_Sensors */
#define MIXED_CHANNEL_DOUT_CURRENT_1    ADC1_CHANNEL_8
#define MIXED_CHANNEL_DOUT_CURRENT_2    ADC1_CHANNEL_0 //ADC2_CHANNEL_0
#define MIXED_CHANNEL_DOUT_CURRENT_3    ADC1_CHANNEL_0
#define MIXED_CHANNEL_DOUT_CURRENT_4    ADC1_CHANNEL_6

/* EANA_Commands */
#define MIXED_PIN_EANA_CMD_1            GPIO_NUM_16
#define MIXED_PIN_EANA_CMD_2            GPIO_NUM_18
#define MIXED_PIN_EANA_CMD_3            GPIO_NUM_20
#define MIXED_PIN_EANA_CMD_4            GPIO_NUM_21

/* SPI */
#define MIXED_SPI_HOST                  SPI2_HOST
#define MIXED_SPI_FREQ                  SPI_MASTER_FREQ_8M
#define MIXED_SPI_PIN_MISO              GPIO_NUM_36
#define MIXED_SPI_PIN_MOSI              GPIO_NUM_48
#define MIXED_SPI_PIN_SCK               GPIO_NUM_37

/* ADC */
#define MIXED_ADC_NB                    (4)
#define MIXED_ADC_PIN_RST               GPIO_NUM_35
#define MIXED_ADC_PIN_CS                GPIO_NUM_34
#define MIXED_ADC_PIN_ALARM             GPIO_NUM_2

/* DAC */
#define MIXED_DAC_NB                    (2)
#define MIXED_DAC_PIN_SYNC_1            GPIO_NUM_4 // CS1
#define MIXED_DAC_PIN_SYNC_2            GPIO_NUM_17 // CS2
#define MIXED_DAC_PIN_LDAC_1            GPIO_NUM_6
#define MIXED_DAC_PIN_LDAC_2            GPIO_NUM_15

/* DOUT */
#define MIXED_PIN_DINS ((1ULL<<MIXED_PIN_DIN_1) | \
                        (1ULL<<MIXED_PIN_DIN_2) | \
                        (1ULL<<MIXED_PIN_DIN_3) | \
                        (1ULL<<MIXED_PIN_DIN_4))

/* DIN */
#define MIXED_PIN_DOUTS ((1ULL<<MIXED_PIN_DOUT_1) | \
                         (1ULL<<MIXED_PIN_DOUT_2) | \
                         (1ULL<<MIXED_PIN_DOUT_3) | \
                         (1ULL<<MIXED_PIN_DOUT_4))


#endif