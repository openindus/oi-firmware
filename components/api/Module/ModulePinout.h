/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file ModulePinout.h
 * @brief Functions for modules
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#define MODULE_PIN_LED                GPIO_NUM_3
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define MODULE_PIN_LED                GPIO_NUM_21
#endif

#define MODULE_PIN_CAN_RX             GPIO_NUM_4
#define MODULE_PIN_CAN_TX             GPIO_NUM_5

#define MODULE_RS_NUM_PORT             UART_NUM_1

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define MODULE_PIN_RS_UART_RX          GPIO_NUM_19
#define MODULE_PIN_RS_UART_TX          GPIO_NUM_20
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define MODULE_PIN_RS_UART_RX          GPIO_NUM_18
#define MODULE_PIN_RS_UART_TX          GPIO_NUM_17
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define MODULE_PIN_RS_UART_RX          GPIO_NUM_16
#define MODULE_PIN_RS_UART_TX          GPIO_NUM_17
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#define MODULE_PIN_OI_ID               GPIO_NUM_1
#define MODULE_PIN_OI_GPIO             GPIO_NUM_0
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define MODULE_PIN_OI_ID               GPIO_NUM_35
#define MODULE_PIN_OI_GPIO             GPIO_NUM_NC
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define MODULE_OI_ID_ADC_CHANNEL       ADC_CHANNEL_0
#define MODULE_OI_ID_ADC_WIDTH         ADC_WIDTH_BIT_12
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define MODULE_OI_ID_ADC_CHANNEL       ADC_CHANNEL_0
#define MODULE_OI_ID_ADC_WIDTH         ADC_WIDTH_BIT_13
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define MODULE_OI_ID_ADC_CHANNEL       ADC_CHANNEL_7
#define MODULE_OI_ID_ADC_WIDTH         ADC_WIDTH_BIT_12
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32S2)
#define MODULE_PIN_CMD_MOSFET_ALIM    GPIO_NUM_2
#elif defined(CONFIG_IDF_TARGET_ESP32)
#define MODULE_PIN_CMD_MOSFET_ALIM    GPIO_NUM_NC
#endif
