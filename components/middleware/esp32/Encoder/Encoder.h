/**
 * @file Encoder.h
 * @brief Encoder class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <stdio.h>
#include "DigitalInputs.h"

class Encoder
{
protected:
    Encoder() {}
    ~Encoder() {}

public:

    /**
     * @brief Encoder initialization
     *
     * @param A Digital input number A
     * @param B Digital input number B
     * @param ppr Pulse per revolution
     * @return int
     */
    static int begin(DIn_Num_t A, DIn_Num_t B, uint32_t ppr);

    /**
     * @brief Encoder end
     *
     */
    static void end(void);

    /**
     * @brief Get the position of the motor
     *
     * @return float position in number of pulses
     */
    static float getPosition(void);

    /**
     * @brief Get the speed of the motor
     *
     * @return float
     */
    static float getSpeed(void);

    /**
     * @brief Get the angle of the motor
     *
     * @return float angle in radians
     */
    static float getAngle(void);
};
