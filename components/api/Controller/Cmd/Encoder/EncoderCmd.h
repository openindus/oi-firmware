/**
 * @file EncoderCmd.h
 * @brief EncoderCmd class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(MODULE_MASTER)

#include "Controller.h"
#include "MasterController.h"
#include "Encoder.h"

class EncoderCmd
{
private:
    Controller *_controller;

public:
    EncoderCmd(Controller *controller) : _controller(controller) {}
    ~EncoderCmd() {}

    /**
     * @brief Encoder initialization
     *
     * @param A Digital input number A
     * @param B Digital input number B
     * @param ppr Pulse per revolution
     * @return int
     */
    int begin(DIn_Num_t A, DIn_Num_t B, uint32_t ppr);

    /**
     * @brief Encoder end
     *
     */
    void end(void);

    /**
     * @brief Get the position of the motor
     *
     * @return float
     */
    float getPosition(void);

    /**
     * @brief Get the speed of the motor
     *
     * @return float
     */
    float getSpeed(void);

    /**
     * @brief Get the angle of the motor
     *
     * @return float
     */
    float getAngle(void);
};

#endif