/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file DiscretePinout.h
 * @brief Callbacks for discrete module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_DISCRETE)

#if defined(CONFIG_IDF_TARGET_ESP32S3)

/* DIN */
#define DISCRETE_NUMBER_OF_DIN  10
#define DISCRETE_PIN_DIN_1      GPIO_NUM_20
#define DISCRETE_PIN_DIN_2      GPIO_NUM_21
#define DISCRETE_PIN_DIN_3      GPIO_NUM_26
#define DISCRETE_PIN_DIN_4      GPIO_NUM_48
#define DISCRETE_PIN_DIN_5      GPIO_NUM_47
#define DISCRETE_PIN_DIN_6      GPIO_NUM_33
#define DISCRETE_PIN_DIN_7      GPIO_NUM_34
#define DISCRETE_PIN_DIN_8      GPIO_NUM_35
#define DISCRETE_PIN_DIN_9      GPIO_NUM_36
#define DISCRETE_PIN_DIN_10     GPIO_NUM_37

/* DOUT */
#define DISCRETE_NUMBER_OF_DOUT 8
#define DISCRETE_PIN_DOUT_1     GPIO_NUM_13
#define DISCRETE_PIN_DOUT_2     GPIO_NUM_10
#define DISCRETE_PIN_DOUT_3     GPIO_NUM_15
#define DISCRETE_PIN_DOUT_4     GPIO_NUM_14
#define DISCRETE_PIN_DOUT_5     GPIO_NUM_16
#define DISCRETE_PIN_DOUT_6     GPIO_NUM_17
#define DISCRETE_PIN_DOUT_7     GPIO_NUM_18
#define DISCRETE_PIN_DOUT_8     GPIO_NUM_19

/* DOUT Sensors */
#define DISCRETE_CHANNEL_DOUT_CURRENT_1     {ADC_UNIT_1, ADC_CHANNEL_0}
#define DISCRETE_CHANNEL_DOUT_CURRENT_2     {ADC_UNIT_1, ADC_CHANNEL_1}
#define DISCRETE_CHANNEL_DOUT_CURRENT_3     {ADC_UNIT_1, ADC_CHANNEL_3}
#define DISCRETE_CHANNEL_DOUT_CURRENT_4     {ADC_UNIT_1, ADC_CHANNEL_4}
#define DISCRETE_CHANNEL_DOUT_CURRENT_5     {ADC_UNIT_1, ADC_CHANNEL_5}
#define DISCRETE_CHANNEL_DOUT_CURRENT_6     {ADC_UNIT_1, ADC_CHANNEL_6}
#define DISCRETE_CHANNEL_DOUT_CURRENT_7     {ADC_UNIT_1, ADC_CHANNEL_7}
#define DISCRETE_CHANNEL_DOUT_CURRENT_8     {ADC_UNIT_1, ADC_CHANNEL_8}

/* AIN */
#define DISCRETE_NUMBER_OF_AIN              2
#define DISCRETE_CHANNEL_AIN_1              {ADC_UNIT_2, ADC_CHANNEL_0}
#define DISCRETE_CHANNEL_AIN_2              {ADC_UNIT_2, ADC_CHANNEL_1}

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

/* DIN */
#define DISCRETE_NUMBER_OF_DIN  10
#define DISCRETE_PIN_DIN_1      GPIO_NUM_41
#define DISCRETE_PIN_DIN_2      GPIO_NUM_46
#define DISCRETE_PIN_DIN_3      GPIO_NUM_36
#define DISCRETE_PIN_DIN_4      GPIO_NUM_26
#define DISCRETE_PIN_DIN_5      GPIO_NUM_33
#define DISCRETE_PIN_DIN_6      GPIO_NUM_45
#define DISCRETE_PIN_DIN_7      GPIO_NUM_35
#define DISCRETE_PIN_DIN_8      GPIO_NUM_38
#define DISCRETE_PIN_DIN_9      GPIO_NUM_34
#define DISCRETE_PIN_DIN_10     GPIO_NUM_37

/* DOUT */
#define DISCRETE_NUMBER_OF_DOUT 8
#define DISCRETE_PIN_DOUT_1     GPIO_NUM_40
#define DISCRETE_PIN_DOUT_2     GPIO_NUM_39
#define DISCRETE_PIN_DOUT_3     GPIO_NUM_42
#define DISCRETE_PIN_DOUT_4     GPIO_NUM_21
#define DISCRETE_PIN_DOUT_5     GPIO_NUM_6
#define DISCRETE_PIN_DOUT_6     GPIO_NUM_7
#define DISCRETE_PIN_DOUT_7     GPIO_NUM_8
#define DISCRETE_PIN_DOUT_8     GPIO_NUM_9 

/* DOUT Sensors */
#define DISCRETE_CHANNEL_DOUT_CURRENT_1     {ADC_UNIT_2, ADC_CHANNEL_1}
#define DISCRETE_CHANNEL_DOUT_CURRENT_2     {ADC_UNIT_2, ADC_CHANNEL_3}
#define DISCRETE_CHANNEL_DOUT_CURRENT_3     {ADC_UNIT_2, ADC_CHANNEL_5}
#define DISCRETE_CHANNEL_DOUT_CURRENT_4     {ADC_UNIT_2, ADC_CHANNEL_9}
#define DISCRETE_CHANNEL_DOUT_CURRENT_5     {ADC_UNIT_2, ADC_CHANNEL_8}
#define DISCRETE_CHANNEL_DOUT_CURRENT_6     {ADC_UNIT_2, ADC_CHANNEL_4}
#define DISCRETE_CHANNEL_DOUT_CURRENT_7     {ADC_UNIT_2, ADC_CHANNEL_2}
#define DISCRETE_CHANNEL_DOUT_CURRENT_8     {ADC_UNIT_2, ADC_CHANNEL_0}

/* AIN */
#define DISCRETE_NUMBER_OF_AIN              1
#define DISCRETE_CHANNEL_AIN_1              {ADC_UNIT_1, ADC_CHANNEL_9}
#define DISCRETE_CHANNEL_AIN_2              {ADC_UNIT_MAX, ADC_CHANNEL_MAX}

#endif

#elif defined(OI_DISCRETE_VE)

/* DIN */
#define DISCRETE_PIN_DIN_1    GPIO_NUM_46
#define DISCRETE_PIN_DIN_2    GPIO_NUM_12
#define DISCRETE_PIN_DIN_3    GPIO_NUM_6
#define DISCRETE_PIN_DIN_4    GPIO_NUM_7
#define DISCRETE_PIN_DIN_5    GPIO_NUM_9
#define DISCRETE_PIN_DIN_6    GPIO_NUM_8
#define DISCRETE_PIN_DIN_7    GPIO_NUM_10
#define DISCRETE_PIN_DIN_8    GPIO_NUM_14
#define DISCRETE_PIN_DIN_9    GPIO_NUM_45
#define DISCRETE_PIN_DIN_10   GPIO_NUM_11

/* DOUT */
#define DISCRETE_PIN_DOUT_1    GPIO_NUM_15
#define DISCRETE_PIN_DOUT_2    GPIO_NUM_41
#define DISCRETE_PIN_DOUT_3    GPIO_NUM_39
#define DISCRETE_PIN_DOUT_4    GPIO_NUM_26
#define DISCRETE_PIN_DOUT_5    GPIO_NUM_34
#define DISCRETE_PIN_DOUT_6    GPIO_NUM_36
#define DISCRETE_PIN_DOUT_7    GPIO_NUM_38
#define DISCRETE_PIN_DOUT_8    GPIO_NUM_37

/* DOUT Sensors*/
#define DISCRETE_CHANNEL_DOUT_CURRENT_1     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_2     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_3     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_4     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_5     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_6     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_7     ADC_CHANNEL_2
#define DISCRETE_CHANNEL_DOUT_CURRENT_8     ADC_CHANNEL_2

/* AIN */
#define DISCRETE_CHANNEL_AIN_1    ADC2_CHANNEL_6
#define DISCRETE_CHANNEL_AIN_2    ADC2_CHANNEL_1 
#define DISCRETE_CHANNEL_AIN_3    ADC2_CHANNEL_5
#define DISCRETE_ADC_REDUCTION_FACTOR     11.6965f

#endif
