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

#if defined(CONFIG_MIXED)

/* DIN */
#define MIXED_PIN_DIN_1                  GPIO_NUM_20
#define MIXED_PIN_DIN_2                  GPIO_NUM_19
#define MIXED_PIN_DIN_3                  GPIO_NUM_46
#define MIXED_PIN_DIN_4                  GPIO_NUM_26

/* DOUT */
#define MIXED_PIN_DOUT_1                  GPIO_NUM_45
#define MIXED_PIN_DOUT_2                  GPIO_NUM_42
#define MIXED_PIN_DOUT_3                  GPIO_NUM_38
#define MIXED_PIN_DOUT_4                  GPIO_NUM_33

/* DOUT_Sensors */
#define MIXED_CHANNEL_DOUT_CURRENT_1      ADC_CHANNEL_3
#define MIXED_CHANNEL_DOUT_CURRENT_2      ADC_CHANNEL_2
#define MIXED_CHANNEL_DOUT_CURRENT_3      ADC_CHANNEL_1
#define MIXED_CHANNEL_DOUT_CURRENT_4      ADC_CHANNEL_0

/* ANA */
#define MIXED_PIN_AIN_1                  GPIO_NUM_6
#define MIXED_CHANNEL_AIN_1              ADC1_CHANNEL_5
#define MIXED_PIN_AIN_2                  GPIO_NUM_7
#define MIXED_CHANNEL_AIN_2              ADC1_CHANNEL_6

/* ADS8664 Pin */
#define MIXED_ADC_NB                      (uint8_t) 4
#define MIXED_ADC_DEFAULT_RES_BITS        (uint8_t) 12
#define MIXED_ADC_PIN_RST                 GPIO_NUM_8
#define MIXED_ADC_PIN_CS                  GPIO_NUM_34
#define MIXED_ADC_PIN_MODE_AIN_1         GPIO_NUM_21
#define MIXED_ADC_PIN_MODE_AIN_2         GPIO_NUM_15
#define MIXED_ADC_PIN_MODE_AIN_3         GPIO_NUM_16
#define MIXED_ADC_PIN_MODE_AIN_4         GPIO_NUM_10

/* DAC8760 Configuration */
#define MIXED_DAC_NB                      (uint8_t) 2
#define MIXED_DAC_PIN_CS1                 GPIO_NUM_39
#define MIXED_DAC_PIN_CS2                 GPIO_NUM_40

/* SPI pins */
#define MIXED_SPI_HOST                    SPI2_HOST
#define MIXED_SPI_FREQ                    SPI_MASTER_FREQ_8M
#define MIXED_PIN_SPI_MISO                GPIO_NUM_37
#define MIXED_PIN_SPI_MOSI                GPIO_NUM_35
#define MIXED_PIN_SPI_SCK                 GPIO_NUM_36

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
