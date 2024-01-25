/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file BrushlessPinout.h
 * @brief Functions for Brushless module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_IDF_TARGET_ESP32S3)

/* GPIO Motor control */
#define BRUSHLESS_PIN_ENABLE_CHIP   GPIO_NUM_10
#define BRUSHLESS_PIN_DIRECTION     GPIO_NUM_16
#define BRUSHLESS_PIN_BRAKE         GPIO_NUM_17
#define BRUSHLESS_PIN_NFAULT        GPIO_NUM_21
#define BRUSHLESS_PIN_FGOUT         GPIO_NUM_18
#define BRUSHLESS_PIN_SPEED_CTRL    GPIO_NUM_15

/* DIN 9-30V */
#define BRUSHLESS_PIN_DIN_1        GPIO_NUM_6
#define BRUSHLESS_PIN_DIN_2        GPIO_NUM_7

/* DIN 3-15V */
#define BRUSHLESS_PIN_CODEURA       GPIO_NUM_8
#define BRUSHLESS_PIN_CODEURB       GPIO_NUM_9

/* DOUT control */
#define BRUSHLESS_PIN_DOUT_1        GPIO_NUM_26
#define BRUSHLESS_PIN_DOUT_2        GPIO_NUM_33
#define BRUSHLESS_PIN_ST_DOUT_1     GPIO_NUM_34
#define BRUSHLESS_PIN_ST_DOUT_2     GPIO_NUM_35

/* AIN */
#define BRUSHLESS_PIN_AIN_1        ADC2_CHANNEL_0    // GPIO_NUM_11 
#define BRUSHLESS_PIN_AIN_2        ADC2_CHANNEL_1    // GPIO_NUM_12
#define BRUSHLESS_PIN_AIN_3        ADC2_CHANNEL_2    // GPIO_NUM_13
#define BRUSHLESS_PIN_AIN_4        ADC2_CHANNEL_3    // GPIO_NUM_14

#endif