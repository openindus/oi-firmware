/**
 * @file MotorDcPidCtrl.cpp
 * @brief MotorDcPidCtrl class implementation with PID position control for DC motors
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDcPidCtrl.h"
#include "pid_ctrl.h"
#include "esp_log.h"
#include <vector>
#include <cmath>

static const char* TAG = "MotorDcPidCtrl";

std::vector<Encoder*> MotorDcPidCtrl::_motorEncoders;
std::vector<pid_ctrl_block_handle_f_t> MotorDcPidCtrl::_pidBlocks;
std::vector<float> MotorDcPidCtrl::_positionSetpoints;
std::vector<float> MotorDcPidCtrl::_currentPositions;
std::vector<float> MotorDcPidCtrl::_positionFeedbacks;

int MotorDcPidCtrl::init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin, const pid_ctrl_config_f_t *pidConfig)
{
    ESP_LOGI(TAG, "Initializing MotorDcPidCtrl with PID position control");

    // Initialize base MotorDc
    int err = MotorDc::init(motorsConfig, faultPin);
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to initialize base MotorDc");
        return err;
    }

    // Initialize PID control blocks for each motor
    _pidBlocks.resize(motorsConfig.size(), nullptr);
    _positionSetpoints.resize(motorsConfig.size(), 0.0f);
    _currentPositions.resize(motorsConfig.size(), 0.0f);
    _positionFeedbacks.resize(motorsConfig.size(), 0.0f);
    _motorEncoders.resize(motorsConfig.size(), nullptr);

    for (size_t i = 0; i < motorsConfig.size(); i++) {
        // Create PID control block with provided configuration
        if (pidConfig != nullptr) {
            err = pid_new_control_block(pidConfig, &_pidBlocks[i]);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to create PID block for motor %d", i);
                return err;
            }
            ESP_LOGI(TAG, "PID block created for motor %d: Kp=%.3f, Ki=%.3f, Kd=%.3f",
                     i, pidConfig->init_param.kp, pidConfig->init_param.ki, pidConfig->init_param.kd);
        } else {
            // Use default PID parameters for position control
            pid_ctrl_config_f_t defaultConfig = {
                .init_param = {
                    .kp = 0.5f,              // Proportional gain for position control
                    .ki = 0.0f,              // Integral gain (can be enabled for zero steady-state error)
                    .kd = 0.1f,              // Derivative gain for damping
                    .max_output = 100.0f,    // Maximum duty cycle percentage
                    .min_output = 0.0f,      // Minimum duty cycle percentage
                    .max_integral = 100.0f,  // Maximum integral windup limit
                    .min_integral = -100.0f, // Minimum integral windup limit
                    .cal_type = PID_CAL_TYPE_POSITIONAL
                }
            };
            err = pid_new_control_block(&defaultConfig, &_pidBlocks[i]);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to create default PID block for motor %d", i);
                return err;
            }
            ESP_LOGI(TAG, "Default PID block created for motor %d: Kp=0.5, Ki=0.0, Kd=0.1", i);
        }
    }

    ESP_LOGI(TAG, "MotorDcPidCtrl initialized successfully");
    return 0;
}

void MotorDcPidCtrl::attachEncoder(MotorNum_t motor, Encoder* encoder)
{
    if (motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _motorEncoders[motor] = encoder;
    ESP_LOGI(TAG, "Encoder attached to motor %d", motor);
}

void MotorDcPidCtrl::detachEncoder(MotorNum_t motor)
{
    if (motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _motorEncoders[motor] = nullptr;
    ESP_LOGI(TAG, "Encoder detached from motor %d", motor);
}

void MotorDcPidCtrl::runToPosition(MotorNum_t motor, MotorDirection_t direction, float positionSetpoint)
{
    if (motor >= _pidBlocks.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    // Update position feedback from encoder
    updatePositionFeedback(motor);
    
    // Set position setpoint for PID controller
    _positionSetpoints[motor] = positionSetpoint;
    
    // Calculate error for PID
    float currentFeedback = _positionFeedbacks[motor];
    float error = positionSetpoint - currentFeedback;
    
    // Compute PID output using position error
    float pidOutput = 0.0f;
    pid_compute(_pidBlocks[motor], error, &pidOutput);
    
    // Clamp PID output to valid duty cycle range
    if (pidOutput > 100.0f) pidOutput = 100.0f;
    if (pidOutput < 0.0f) pidOutput = 0.0f;
    
    // Run motor with computed duty cycle
    run(motor, direction, pidOutput);
    
    ESP_LOGD(TAG, "Motor %d: position setpoint=%.1f, feedback=%.1f, error=%.1f, duty=%.1f%%",
             motor, positionSetpoint, currentFeedback, error, pidOutput);
}

float MotorDcPidCtrl::getCurrentPosition(MotorNum_t motor)
{
    if (motor >= _currentPositions.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return 0.0f;
    }

    // If an encoder is attached, use encoder feedback
    Encoder* enc = _motorEncoders[motor];
    if (enc != nullptr) {
        _currentPositions[motor] = static_cast<float>(enc->getPulses());
    }

    return _currentPositions[motor];
}

void MotorDcPidCtrl::resetPosition(MotorNum_t motor)
{
    if (motor >= _currentPositions.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _currentPositions[motor] = 0.0f;
    _positionFeedbacks[motor] = 0.0f;
    
    // Reset encoder if attached
    Encoder* enc = _motorEncoders[motor];
    if (enc != nullptr) {
        enc->reset();
    }
    
    // Reset PID integral term
    pid_reset_ctrl_block(_pidBlocks[motor]);
    
    ESP_LOGI(TAG, "Position reset for motor %d", motor);
}

void MotorDcPidCtrl::setCurrentPosition(MotorNum_t motor, float position)
{
    if (motor >= _currentPositions.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return;
    }

    _currentPositions[motor] = position;
    ESP_LOGD(TAG, "Current position set for motor %d: %.1f", motor, position);
}

float MotorDcPidCtrl::updatePositionFeedback(MotorNum_t motor)
{
    if (motor >= _motorEncoders.size()) {
        ESP_LOGE(TAG, "Invalid motor number: %d", motor);
        return 0.0f;
    }

    Encoder* enc = _motorEncoders[motor];
    if (enc != nullptr) {
        // Update position feedback from encoder pulses
        _positionFeedbacks[motor] = static_cast<float>(enc->getPulses());
        return _positionFeedbacks[motor];
    }

    return _positionFeedbacks[motor];
}