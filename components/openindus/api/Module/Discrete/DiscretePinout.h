/**
 * @file DiscretePinout.h
 * @brief Discrete Pinout
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once
#if defined(CONFIG_OI_DISCRETE) || defined(CONFIG_OI_DISCRETE_VE)
#if defined(CONFIG_OI_DISCRETE)

/* DIN */
#define DISCRETE_GPIO_DIN_1             GPIO_NUM_20
#define DISCRETE_GPIO_DIN_2             GPIO_NUM_21
#define DISCRETE_GPIO_DIN_3             GPIO_NUM_26
#define DISCRETE_GPIO_DIN_4             GPIO_NUM_48
#define DISCRETE_GPIO_DIN_5             GPIO_NUM_47
#define DISCRETE_GPIO_DIN_6             GPIO_NUM_33
#define DISCRETE_GPIO_DIN_7             GPIO_NUM_34
#define DISCRETE_GPIO_DIN_8             GPIO_NUM_35
#define DISCRETE_GPIO_DIN_9             GPIO_NUM_36
#define DISCRETE_GPIO_DIN_10            GPIO_NUM_37

/* DOUT */
#define DISCRETE_GPIO_DOUT_1            GPIO_NUM_13
#define DISCRETE_GPIO_DOUT_2            GPIO_NUM_10
#define DISCRETE_GPIO_DOUT_3            GPIO_NUM_15
#define DISCRETE_GPIO_DOUT_4            GPIO_NUM_14
#define DISCRETE_GPIO_DOUT_5            GPIO_NUM_16
#define DISCRETE_GPIO_DOUT_6            GPIO_NUM_17
#define DISCRETE_GPIO_DOUT_7            GPIO_NUM_18
#define DISCRETE_GPIO_DOUT_8            GPIO_NUM_19

#elif defined(CONFIG_OI_DISCRETE_VE)

/* DIN */
#define DISCRETE_GPIO_DIN_1             GPIO_NUM_20
#define DISCRETE_GPIO_DIN_2             GPIO_NUM_21
#define DISCRETE_GPIO_DIN_3             GPIO_NUM_26
#define DISCRETE_GPIO_DIN_4             GPIO_NUM_31 // Different GPIO pin compared to CONFIG_OI_DISCRETE
#define DISCRETE_GPIO_DIN_5             GPIO_NUM_32 // Different GPIO pin compared to CONFIG_OI_DISCRETE
#define DISCRETE_GPIO_DIN_6             GPIO_NUM_33
#define DISCRETE_GPIO_DIN_7             GPIO_NUM_34
#define DISCRETE_GPIO_DIN_8             GPIO_NUM_35
#define DISCRETE_GPIO_DIN_9             GPIO_NUM_36
#define DISCRETE_GPIO_DIN_10            GPIO_NUM_37

/* DOUT */
#define DISCRETE_GPIO_DOUT_1            GPIO_NUM_13
#define DISCRETE_GPIO_DOUT_2            GPIO_NUM_10
#define DISCRETE_GPIO_DOUT_3            GPIO_NUM_15
#define DISCRETE_GPIO_DOUT_4            GPIO_NUM_14
#define DISCRETE_GPIO_DOUT_5            GPIO_NUM_16
#define DISCRETE_GPIO_DOUT_6            GPIO_NUM_17
#define DISCRETE_GPIO_DOUT_7            GPIO_NUM_18
#define DISCRETE_GPIO_DOUT_8            GPIO_NUM_19

#endif

#define DISCRETE_ADC_CHANNEL_DOUT_1     ADC_CHANNEL_0
#define DISCRETE_ADC_CHANNEL_DOUT_2     ADC_CHANNEL_1
#define DISCRETE_ADC_CHANNEL_DOUT_3     ADC_CHANNEL_3
#define DISCRETE_ADC_CHANNEL_DOUT_4     ADC_CHANNEL_4
#define DISCRETE_ADC_CHANNEL_DOUT_5     ADC_CHANNEL_5
#define DISCRETE_ADC_CHANNEL_DOUT_6     ADC_CHANNEL_6
#define DISCRETE_ADC_CHANNEL_DOUT_7     ADC_CHANNEL_7
#define DISCRETE_ADC_CHANNEL_DOUT_8     ADC_CHANNEL_8
#define DISCRETE_ADC_UNIT_DOUT          ADC_UNIT_1

/* AIN */
#define DISCRETE_ADC_CHANNEL_AIN_1      ADC_CHANNEL_0
#define DISCRETE_ADC_CHANNEL_AIN_2      ADC_CHANNEL_1
#define DISCRETE_ADC_UNIT_AIN           ADC_UNIT_2

#endif
