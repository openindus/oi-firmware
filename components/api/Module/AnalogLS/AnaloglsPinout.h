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

#if defined(OI_ANALOG_LS)

/* ADC */
#define ADC_START   GPIO_NUM_38
#define ADC_DRDY    GPIO_NUM_33
#define ADC_RESET   GPIO_NUM_47

#define MISO   GPIO_NUM_34
#define MOSI   GPIO_NUM_36
#define SCLK   GPIO_NUM_35
#define CS     GPIO_NUM_37

/* Temperature sensor */
#define SDA     GPIO_NUM_39
#define SCL     GPIO_NUM_40
#define OS_INT  GPIO_NUM_21

/* Multiplexer */
#define SEL0    GPIO_NUM_48
#define SEL1    GPIO_NUM_26

#endif