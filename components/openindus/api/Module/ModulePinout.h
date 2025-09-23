/**
 * @file ModulePinout.h
 * @brief Module Pinout
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

/* BUS-ID */
#define MODULE_ADC_CHANNEL_BUS_ID       ADC_CHANNEL_2

/* BUS-GPIO */
#define MODULE_GPIO_BUS_GPIO            GPIO_NUM_38

/* CAN */
#define MODULE_GPIO_CAN_TX              GPIO_NUM_39
#define MODULE_GPIO_CAN_RX              GPIO_NUM_40

/* RS */
#define MODULE_UART_RS_PORT             UART_NUM_1
#define MODULE_GPIO_RS_UART_TX          GPIO_NUM_41
#define MODULE_GPIO_RS_UART_RX          GPIO_NUM_42

/* LED */
#define MODULE_GPIO_LED                 GPIO_NUM_45

/* CMD ALIM */
#define MODULE_GPIO_CMD_MOSFET_ALIM     GPIO_NUM_46