/**
 * @file DcMotorPidCtrl.cpp
 * @brief PID position control example for OI-DC (DC Motor Controller) module
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2026] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "OpenIndus.h"
#include "Arduino.h"

#define POSITION_TOLERANCE  10.0f // Tolerance in pulses to consider the target reached
#define MOVE_TIMEOUT_MS     5000 // Maximum time to reach a target before giving up (ms)

Dc dc;

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

void setup(void)
{
    dc.setPidParams(MOTOR_1, &pidParams); // Apply custom PID gains to motor 1 

    printf("PID position control example started\n");
}

void loop(void)
{
    for (size_t i = 0; i < targetCount; i++) {
        float target = targets[i];

        printf("Moving MOTOR_1 to %.0f pulses\n", target);
        dc.moveTo(MOTOR_1, target);

        /* Poll position until target reached or timeout */
        uint32_t startMs = millis();
        while (true) {
            float pos = dc.getPosition(MOTOR_1);
            float error = target - pos;

            printf("Position: %.1f  Error: %.1f\n", pos, error);

            if (fabsf(error) <= POSITION_TOLERANCE) {
                printf("Target %.0f reached (position: %.1f)\n", target, pos);
                break;
            }

            if ((millis() - startMs) >= MOVE_TIMEOUT_MS) {
                printf("Timeout reaching target %.0f (position: %.1f)\n", target, pos);
                dc.stop(MOTOR_1);
                break;
            }

            delay(50);
        }

        delay(1000);  // Hold position for 1 second before next target
    }
}
