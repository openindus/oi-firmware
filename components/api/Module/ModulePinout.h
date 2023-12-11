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

/* ID */
#define MODULE_OI_ID_ADC_CHANNEL        ADC1_GPIO3_CHANNEL
#define MODULE_OI_ID_ADC_WIDTH          ADC_WIDTH_BIT_12

/* GPIO */
#define MODULE_PIN_OI_GPIO              GPIO_NUM_38

/* CAN */
#define MODULE_PIN_CAN_TX               GPIO_NUM_39
#define MODULE_PIN_CAN_RX               GPIO_NUM_40

/* RS */
#define MODULE_RS_NUM_PORT              UART_NUM_1
#define MODULE_PIN_RS_UART_TX           GPIO_NUM_41
#define MODULE_PIN_RS_UART_RX           GPIO_NUM_20

/* LED */
#define MODULE_PIN_LED                  GPIO_NUM_45

/* CMD ALIM */
#define MODULE_PIN_CMD_MOSFET_ALIM      GPIO_NUM_46
