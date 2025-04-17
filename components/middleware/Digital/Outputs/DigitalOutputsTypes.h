/**
 * @file DigitalOutputsTypes.h
 * @brief Digital outputs types
 * @author kévin.lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

/**
 * @brief Digital Outputs Numbers
 *
 */
typedef enum {
    DOUT_1 = 0,
    DOUT_2,
    DOUT_3,
    DOUT_4,
    DOUT_5,
    DOUT_6,
    DOUT_7,
    DOUT_8,
    DOUT_MAX
} DOut_Num_t;

typedef enum { DOUT_MODE_DIGITAL = 0, DOUT_MODE_PWM } DOut_Mode_t;