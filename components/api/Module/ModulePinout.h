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

#if defined(CONFIG_IDF_TARGET_ESP32S3)

/* ID */
#define MODULE_OI_ID_ADC_CHANNEL        ADC1_CHANNEL_2
#define MODULE_OI_ID_ADC_WIDTH          ADC_WIDTH_BIT_12

/* GPIO */
#define MODULE_PIN_OI_GPIO              GPIO_NUM_38

/* CAN */
#define MODULE_PIN_CAN_TX               GPIO_NUM_39
#define MODULE_PIN_CAN_RX               GPIO_NUM_40

/* RS */
#define MODULE_RS_NUM_PORT              UART_NUM_1
#define MODULE_PIN_RS_UART_TX           GPIO_NUM_41
#define MODULE_PIN_RS_UART_RX           GPIO_NUM_42

/* LED */
#define MODULE_PIN_LED                  GPIO_NUM_45

/* CMD ALIM */
#define MODULE_PIN_CMD_MOSFET_ALIM      GPIO_NUM_46

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

/* ID */
#define MODULE_OI_ID_ADC_CHANNEL        ADC1_GPIO1_CHANNEL
#define MODULE_OI_ID_ADC_WIDTH          ADC_WIDTH_BIT_13

/* GPIO */
#define MODULE_PIN_OI_GPIO              GPIO_NUM_0

/* CAN */
#define MODULE_PIN_CAN_TX               GPIO_NUM_5
#define MODULE_PIN_CAN_RX               GPIO_NUM_4

/* RS */
#define MODULE_RS_NUM_PORT              UART_NUM_1
#define MODULE_PIN_RS_UART_TX           GPIO_NUM_17
#define MODULE_PIN_RS_UART_RX           GPIO_NUM_18

/* LED */
#define MODULE_PIN_LED                  GPIO_NUM_3

/* CMD ALIM */
#define MODULE_PIN_CMD_MOSFET_ALIM      GPIO_NUM_2

#endif