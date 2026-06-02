/**
 * @file MotorDcPidCtrl.h
 * @brief MotorDcPidCtrl class definition with PID position control for DC motors
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "MotorDc.h"
#include "pid_ctrl.h"

/**
 * @class MotorDcPidCtrl
 * @brief DC Motor control class with PID position control
 * 
 * This class extends MotorDc to add PID-based position control capability.
 * It uses the espressif pid_ctrl component for implementing a position loop.
 * 
 * Position control uses encoder feedback to maintain a target position.
 * The PID controller adjusts the motor duty cycle to minimize position error.
 */
class MotorDcPidCtrl : public MotorDc
{
protected:
    /**
     * @brief Initialize the motor with PID position control parameters
     * 
     * @param motorsConfig Vector of motor pin configurations
     * @param faultPin Fault detection GPIO pin
     * @param pidConfig PID configuration parameters (optional, uses defaults if nullptr)
     * @return int 0 on success, error code otherwise
     */
    static int init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig = nullptr);

public:
    /**
     * @brief Run the motor to a target position using PID control
     * 
     * @param motor Motor number
     * @param direction Motor direction (FORWARD/REVERSE)
     * @param positionSetpoint Target position in encoder counts or degrees
     */
    static void runToPosition(MotorNum_t motor, MotorDirection_t direction, float positionSetpoint);

    /**
     * @brief Get the current position of a motor
     * 
     * @param motor Motor number
     * @return float Current position in encoder counts or degrees
     */
    static float getCurrentPosition(MotorNum_t motor);

    /**
     * @brief Set the current position of a motor (used for position feedback reset)
     * 
     * @param motor Motor number
     * @param position Current position in encoder counts or degrees
     */
    static void setCurrentPosition(MotorNum_t motor, float position);

    /**
     * @brief Reset position feedback and PID state for a motor
     * 
     * @param motor Motor number
     */
    static void resetPosition(MotorNum_t motor);

private:
    static std::vector<pid_ctrl_block_handle_f_t> _pidBlocks;
    static std::vector<float> _positionSetpoints;
    static std::vector<float> _currentPositions;
    static std::vector<float> _positionFeedbacks;
};
