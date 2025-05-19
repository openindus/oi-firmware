/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelayPinout.h
 * @brief Callbacks for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_RELAY_LP)

#define RELAY_LP_PHASE_CMD1          GPIO_NUM_38
#define RELAY_LP_PHASE_CMD2          GPIO_NUM_37
#define RELAY_LP_PHASE_CMD3          GPIO_NUM_36
#define RELAY_LP_PHASE_CMD4          GPIO_NUM_35
#define RELAY_LP_PHASE_CMD5          GPIO_NUM_34
#define RELAY_LP_PHASE_CMD6          GPIO_NUM_33

#define Relay_PIN_CMD_MOSFET          GPIO_NUM_2

#elif defined(OI_RELAY_HP)

#define RELAY_HP_PHASE_CMD1          GPIO_NUM_13
#define RELAY_HP_PHASE_CMD2          GPIO_NUM_12
#define RELAY_HP_PHASE_CMD3          GPIO_NUM_11
#define RELAY_HP_PHASE_CMD4          GPIO_NUM_10

#define Relay_PIN_CMD_MOSFET          GPIO_NUM_2

#define RELAY_HP_TEMPSENSOR_ALERT    PIO_NUM_34
#define RELAY_HP_TEMPSENSOR_ADDR     0x49

#endif
