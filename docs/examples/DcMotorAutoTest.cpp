/**
 * @file DcMotorAutoTest.cpp
 * @author Georges de Massol
 * @brief Automated test for OI-DC (DC Motor Controller) module
 * 
 * This file provides an automated test suite for the OpenIndus DC Motor Controller module.
 * It tests each of the 4 motor outputs sequentially in both forward and reverse directions,
 * verifying proper current readings and ensuring no cross-talk between channels.
 * 
 * Test Setup:
 * - Power supply: 24V DC
 * - Load per channel: 25Ω resistor (expected current: 0.96A)
 * - Tolerance: ±10% (to account for resistor tolerance)
 * 
 * Hardware Wiring Diagram:
 * 
 *     OI-DC Module Pinout & Test Configuration
 *     =========================================
 * 
 *     Power Supply (24V)   
 *          +                        R1+     R2+            R3+      R4+
 *          |                         |       |              |        |
 *          |                         |       |              |        |     
 *     +----|-------------------------+-------+--------------+--------+----+
 *     |    |        |       |        |       |      |       |        |    |
 *     |  9V-30V   9V-30V   /B2      /A2      |     /B1     /A1       |    |
 *     |    |        |       |        |       |      |       |        |    |
 *     |   VIN      VIN     DIN4    HB4_2   HB3_2   DIN2   HB2_2   HB1_2   |
 *     |                                                                   |
 *     |   GND      GND     DIN3    HB4_1   HB3_1   DIN1   HB2_1   HB1_1   |
 *     |    |        |       |        |       |      |       |        |    |
 *     |    |        |       B2       A2      |      B1      A1       |    |
 *     |    |        |       |        |       |      |       |        |    |
 *     +----+-------------------------+-------+--------------+--------+----+
 *          |                         |       |              |        |
 *          |                         |       |              |        |
 *          -                        R1-     R2-            R3-      R4-
 *         GND
 * 
 * Motor Channel Mapping:
 * ----------------------
 * MOTOR_1: HB1_1(-) / HB1_2(+) → 25Ω resistor
 * MOTOR_2: HB2_1(-) / HB2_2(+) → 25Ω resistor
 * MOTOR_3: HB3_1(-) / HB3_2(+) → 25Ω resistor
 * MOTOR_4: HB4_1(-) / HB4_2(+) → 25Ω resistor
 * 
 * Expected Results:
 * -----------------
 * - Active motor: ~0.96A (24V / 25Ω)
 * - Inactive motors: 0A
 * - Tolerance: ±0.096A (10% of expected current)
 * 
 * If any motor shows unexpected current (e.g., inactive motors drawing current),
 * the test will log an error indicating a potential wiring issue or module fault.
 * The reverse direction is also tested, 
 * since current measurement account for direction.
 * If direction is inverted, the current measurement will be 0 instead of ~0.96A.
 *
 * @date November 2025
 */

#include "OpenIndus.h"
#include "Arduino.h"
#include "ArduinoModbus.h"

Dc dc;

static const char TAG[] = "Main";

static const float POWER_SUPPLY_VOLTAGE = 24.0f; // V
static const float RESISTOR_VALUE = 25.0f; // Ohm
static const float EXPECTED_MOTOR_CURRENT = (POWER_SUPPLY_VOLTAGE / RESISTOR_VALUE); // A
static const float MOTOR_CURRENT_TOLERANCE = EXPECTED_MOTOR_CURRENT * 0.10f; // A (10% resistor tolerance)
static const float INACTIVE_MOTOR_TOLERANCE = 0.001f; // A

bool readAndLogCurrents(const char* label, const int activeMotorIndex = -1)
{
    float currents[4];
    currents[0] = dc.getCurrent(MOTOR_1);
    currents[1] = dc.getCurrent(MOTOR_2);
    currents[2] = dc.getCurrent(MOTOR_3);
    currents[3] = dc.getCurrent(MOTOR_4);

    ESP_LOGI(TAG, "Motor_%d %s: %.3f %.3f %.3f %.3f A", activeMotorIndex + 1,
        label, currents[0], currents[1], currents[2], currents[3]);

    bool allPassed = true;
    const char* motorNames[] = {"MOTOR_1", "MOTOR_2", "MOTOR_3", "MOTOR_4"};
    
    // Check if this is a STOPPED state (expecting all motors at 0A)
    bool isStopped = (strcmp(label, "STOPPED") == 0);
    
    for (int i = 0; i < 4; i++) {
        float expectedCurrent;
        float tolerance;
        
        if (isStopped) {
            // All motors should be at 0A when stopped
            expectedCurrent = 0.0;
            tolerance = INACTIVE_MOTOR_TOLERANCE;
        } else {
            // Active motor should be at 0.9A, others at 0A
            expectedCurrent = (i == activeMotorIndex) ? EXPECTED_MOTOR_CURRENT : 0.0;
            tolerance = (i == activeMotorIndex) ? MOTOR_CURRENT_TOLERANCE : INACTIVE_MOTOR_TOLERANCE;
        }
        
        float diff = fabs(currents[i] - expectedCurrent);
        
        if (diff > tolerance) {
            ESP_LOGE(TAG, "%s FAILED: %.3f A (expected: %.3f A +/- %.3f A, diff: %.3f A)", 
                     motorNames[i], currents[i], expectedCurrent, tolerance, diff);
            allPassed = false;
            
            if (!isStopped && i != activeMotorIndex && currents[i] > tolerance) {
                ESP_LOGW(TAG, "Possible inversion or short detected!");
            }
        }
    }
    
    if (isStopped && !allPassed) {
        ESP_LOGW(TAG, "Some motors still drawing current after stop!");
    }
    
    return allPassed;
}

bool checkCurrent(const MotorNum_t motor, const char* motorName, const float expectedCurrent, const float tolerance)
{
    float current = dc.getCurrent(motor);
    float diff = fabs(current - expectedCurrent);
    
    if (diff <= tolerance) {
        ESP_LOGI(TAG, "%s current OK: %.3f A (expected: %.3f A +/- %.3f A)", motorName, current, expectedCurrent, tolerance);
        return true;
    } else {
        ESP_LOGE(TAG, "%s current FAILED: %.3f A (expected: %.3f A +/- %.3f A, diff: %.3f A)", 
                 motorName, current, expectedCurrent, tolerance, diff);
        return false;
    }
}

void setup(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);

    // Array of motors for sequential operation
    const MotorNum_t motors[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4};

    delay(100);

    // Run each motor sequentially forward then backward
    for (int i = 0; i < 4; i++) {
        dc.run(motors[i], FORWARD, 100);
        // delay(100);
        readAndLogCurrents("FORWARD", i);
        // delay(900);

        dc.run(motors[i], REVERSE, 100);
        // delay(100);
        readAndLogCurrents("REVERSE", i);
        // delay(900);

        //stop motor
        dc.stop(motors[i]);
        // delay(100);
        readAndLogCurrents("STOPPED", i);
        
        // Yield to allow IDLE task to run and feed watchdog
        delay(100);
    }
}

void loop(void)
{
    delay(1000);
}