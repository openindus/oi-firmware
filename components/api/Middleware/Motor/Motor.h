/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Motor.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

typedef enum {
    MOTOR_1 = 0,
#if !defined(OI_STEPPER_VE)
    MOTOR_2,
#endif
#if defined(OI_DC)
    MOTOR_3,
    MOTOR_4,
#endif
    MOTOR_MAX,
} MotorNum_t;


/**
 * @brief Motors directions
 * 
 */
typedef enum {
    FORWARD = 1,
    REVERSE = 0,
} MotorDirection_t;

class Motor
{

};