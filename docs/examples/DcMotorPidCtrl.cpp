/**
 * @file DcMotorPidCtrl.cpp
 * @brief PID position control example for OI-DC (DC Motor Controller) module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 *
 * This example demonstrates closed-loop PID position control using the
 * OpenIndus DC module with a quadrature encoder:
 *  - Tune PID parameters at runtime
 *  - Move motor to an absolute target position (in encoder pulses)
 *  - Read back the current encoder position
 *  - Stop position control and brake the motor
 *
 * Hardware Wiring Diagram:
 *
 *     OI-DC Module — PID Position Control
 *     =====================================
 *
 *     Power Supply (24V)
 *          +
 *          |
 *     +----|-------------------------+
 *     |  9V-30V                     |
 *     |   VIN        OI-DC          |
 *     |                             |
 *     |   HB1_1 ─── Motor A ─────── |
 *     |   HB1_2 ─── Motor B ─────── |
 *     |                             |
 *     |   DIN_1 ─── Encoder A ───── |
 *     |   DIN_2 ─── Encoder B ───── |
 *     |                             |
 *     |   GND                       |
 *     +----|-------------------------+
 *          |
 *          -
 *         GND
 *
 * Encoder is connected to DIN_1 (channel A) and DIN_2 (channel B).
 * Motor 1 is driven through HB1_1 / HB1_2 half-bridge outputs.
 *
 * @note The encoder is attached on the slave (OI-DC) side automatically
 *       during Dc::init(). The master communicates position commands over
 *       the OpenIndus bus.
 */

#include "OpenIndus.h"
#include "Arduino.h"

Dc dc;  /* DC motor controller (master side) */

static const char TAG[] = "Main";

/* PID parameters — adjust to your motor/load */
static const pid_ctrl_parameter_f_t pidParams = {
    .kp          = 2.5f,
    .ki          = 0.1f,
    .kd          = 0.05f,
    .max_output  =  100.0f,
    .min_output  = -100.0f,
    .max_integral =  20.0f,
    .min_integral = -20.0f,
    .cal_type    = PID_CAL_TYPE_POSITIONAL,
};

/* Sequence of absolute target positions (encoder pulses) */
static const float targets[] = { 500.0f, -500.0f, 1000.0f, 0.0f };
static const size_t targetCount = sizeof(targets) / sizeof(targets[0]);

/* Tolerance in pulses to consider the target reached */
#define POSITION_TOLERANCE  10.0f

/* Maximum time to reach a target before giving up (ms) */
#define MOVE_TIMEOUT_MS     5000

void setup(void)
{
    /* Apply custom PID gains to motor 1 */
    dc.setPidParams(MOTOR_1, &pidParams);

    ESP_LOGI(TAG, "PID position control example started");
}

void loop(void)
{
    for (size_t i = 0; i < targetCount; i++) {
        float target = targets[i];

        ESP_LOGI(TAG, "Moving MOTOR_1 to %.0f pulses", target);
        dc.moveTo(MOTOR_1, target);

        /* Poll position until target reached or timeout */
        uint32_t startMs = millis();
        while (true) {
            float pos = dc.getPosition(MOTOR_1);
            float error = target - pos;

            ESP_LOGD(TAG, "Position: %.1f  Error: %.1f", pos, error);

            if (fabsf(error) <= POSITION_TOLERANCE) {
                ESP_LOGI(TAG, "Target %.0f reached (position: %.1f)", target, pos);
                break;
            }

            if ((millis() - startMs) >= MOVE_TIMEOUT_MS) {
                ESP_LOGW(TAG, "Timeout reaching target %.0f (position: %.1f)", target, pos);
                dc.stop(MOTOR_1);
                break;
            }

            delay(50);
        }

        delay(1000);  /* Hold position for 1 second before next target */
    }
}
