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
#include "Encoder.h"
#include <vector>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @class MotorDcPidCtrl
 * @brief DC Motor control class with closed-loop PID position control.
 */
class MotorDcPidCtrl : public MotorDc
{
public:
    /**
     * @brief Attach an encoder to a motor for position feedback.
     * @param motor Motor number
     * @param encoder Pointer to an initialized Encoder instance
     */
    static void attachEncoder(MotorNum_t motor, Encoder* encoder);

    /**
     * @brief Detach the encoder from a motor and stop position control.
     * @param motor Motor number
     */
    static void detachEncoder(MotorNum_t motor);

    /**
     * @brief Start closed-loop position control toward an absolute target.
     * @param motor    Motor number
     * @param position Target position in encoder pulses (from last encoder reset)
     */
    static void moveTo(MotorNum_t motor, float position);

    /**
     * @brief Stop position control and brake the motor immediately.
     * @param motor Motor number
     */
    static void stop(MotorNum_t motor);

    /**
     * @brief Get the current encoder position.
     * @param motor Motor number
     * @return float Current position in encoder pulses
     */
    static float getPosition(MotorNum_t motor);

    /**
     * @brief Update PID parameters for a motor at runtime.
     * @param motor  Motor number
     * @param params Pointer to the new PID parameter structure
     */
    static void setPidParams(MotorNum_t motor, const pid_ctrl_parameter_f_t* params);

protected:
    /**
     * @brief Initialize DC motors with PID position control.
     * @param motorsConfig Vector of motor pin configurations
     * @param faultPin     GPIO for the hardware fault input
     * @param pidConfig    Initial PID configuration (optional; defaults to Kp=1, Ki=0, Kd=0)
     * @return int 0 on success, non-zero on error
     */
    static int init(std::vector<MotorDC_PinConfig_t> motorsConfig,
        gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig = nullptr);

private:
    static std::vector<Encoder*>                   _motorEncoders;
    static std::vector<pid_ctrl_block_handle_f_t>  _pidHandles;
    static std::vector<float>                      _targetPositions;
    static std::vector<bool>                       _controlActive;
    static TaskHandle_t                            _pidTask;

    static void _pidControlTask(void* arg);
};
