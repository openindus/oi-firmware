/**
 * @file DcConfig.h
 * @brief DC Motor configuration
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_OI_DC)

/* DIN GPIOs pins */   
#define DC_GPIO_PIN_DIN_1           GPIO_NUM_7
#define DC_GPIO_PIN_DIN_2           GPIO_NUM_8
#define DC_GPIO_PIN_DIN_3           GPIO_NUM_6
#define DC_GPIO_PIN_DIN_4           GPIO_NUM_5
#define DC_GPIO_PIN_DIN_5           GPIO_NUM_2
#define DC_GPIO_PIN_DIN_6           GPIO_NUM_4
#define DC_GPIO_PIN_DIN_7           GPIO_NUM_10
#define DC_GPIO_PIN_DIN_8           GPIO_NUM_9

/* Motor 1 GPIOs pins */
#define DC_MOTOR1_IN1               GPIO_NUM_18
#define DC_MOTOR1_IN2               GPIO_NUM_16 
#define DC_MOTOR1_DISABLE           GPIO_NUM_14

/* Motor 2 GPIOs pins */
#define DC_MOTOR2_IN1               GPIO_NUM_26
#define DC_MOTOR2_IN2               GPIO_NUM_21 
#define DC_MOTOR2_DISABLE           GPIO_NUM_20

/* Motor 3 GPIOs pins */
#define DC_MOTOR3_IN1               GPIO_NUM_34
#define DC_MOTOR3_IN2               GPIO_NUM_33 
#define DC_MOTOR3_DISABLE           GPIO_NUM_47

/* Motor 4 GPIOs pins */
#define DC_MOTOR4_IN1               GPIO_NUM_37
#define DC_MOTOR4_IN2               GPIO_NUM_36 
#define DC_MOTOR4_DISABLE           GPIO_NUM_35

/* Fault pin */
#define DC_MOTOR_FAULT              GPIO_NUM_4

#endif