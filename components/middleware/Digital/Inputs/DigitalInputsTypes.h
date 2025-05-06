/**
 * @file DigitalInputsTypes.h
 * @brief Digital Inputs types
 * @author kévin.lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

/**
 * @brief Digital Inputs Numbers
 *
 */
typedef enum {
    DIN_1 = 0,
    DIN_2,
    DIN_3,
    DIN_4,
#if !defined(OI_STEPPER) and !defined(OI_STEPPER_VE)
    DIN_5,
    DIN_6,
    DIN_7,
    DIN_8,
    DIN_9,
    DIN_10,
#endif
    DIN_MAX
} DIn_Num_t;

/**
 * @brief Digital Inputs Logic
 *
 */
typedef enum { 
    ACTIVE_LOW = 0, 
    ACTIVE_HIGH = 1 
} Logic_t;

/**
 * @brief Digital Inputs Interrupts Modes
 *
 */
typedef enum { 
    NONE_MODE = 0, 
    RISING_MODE, 
    FALLING_MODE, 
    CHANGE_MODE 
} InterruptMode_t;

/**
 * @brief Function prototype for attachInterrupt callbacks
 *
 */
typedef void (*IsrCallback_t)(void *);