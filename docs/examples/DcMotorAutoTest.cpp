/**
 * @file DcMotorAutoTest.cpp
 * @author Georges de Massol
 * @brief Automated test for OI-DC (DC Motor Controller) module
 * 
 * This file provides an automated test suite for the OpenIndus DC Motor Controller module.
 * It tests each of the 4 motor outputs sequentially in both forward and reverse directions,
 * verifying proper current readings and ensuring no cross-talk between channels.
 * 
 * WARNING:
 * --------
 * Do not plug digital inputs (sensors, switches, etc.) on DIN pins 5 to 8 during this test. 
 * These pins are used for motors and may create a short circuit if connected.
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

bool readAndLogCurrents(const char* label, const int activeMotorIndex = -1, float percentage = 100.0f)
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
            expectedCurrent = (i == activeMotorIndex) ? (EXPECTED_MOTOR_CURRENT * percentage / 100.0f) : 0.0;
            tolerance = (i == activeMotorIndex) ? (MOTOR_CURRENT_TOLERANCE * percentage / 100.0f) : INACTIVE_MOTOR_TOLERANCE;
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

bool checkAndPrintDInValues(const char* label, const int activeMotorIndex = -1)
{
    // measure and display all eight gpios called DC_GPIO_PIN_DIN_1 to DC_GPIO_PIN_DIN_8
    bool dinValues[8];
    for (int j = 1; j <= 8; j++) {
        dinValues[j - 1] = dc.digitalRead(static_cast<DIn_Num_t>(j - 1));
    }

    // Log the DIn values
    ESP_LOGI(TAG, "DIn values: %d %d %d %d %d %d %d %d", dinValues[0], dinValues[1], dinValues[2], dinValues[3], dinValues[4], dinValues[5], dinValues[6], dinValues[7]);
    
    // Validate DIN values based on motor state
    // Expected mapping (based on log analysis):
    // Motor 1: No associated DIns
    // Motor 2 FORWARD: DIN5 (index 4), Motor 2 REVERSE: DIN6 (index 5)
    // Motor 3: No associated DIns 
    // Motor 4 FORWARD: DIN7 (index 6), Motor 4 REVERSE: DIN8 (index 7)
    
    bool isStopped = (strcmp(label, "STOPPED") == 0);
    bool isForward = (strcmp(label, "FORWARD") == 0);
    
    bool allPassed = true;
    
    if (isStopped) {
        // All DIns should be LOW when all motors are stopped
        for (int i = 0; i < 8; i++) {
            if (dinValues[i]) {
                ESP_LOGE(TAG, "DIN%d FAILED: Expected LOW when stopped, got HIGH", i + 1);
                allPassed = false;
            }
        }
    } else if (activeMotorIndex >= 0) {
        // Check motor-specific DIN expectations
        int expectedDinIndex = -1;
        
        if (activeMotorIndex == 1) { // MOTOR_2
            expectedDinIndex = isForward ? 4 : 5; // DIN5 or DIN6
        } else if (activeMotorIndex == 3) { // MOTOR_4
            expectedDinIndex = isForward ? 6 : 7; // DIN7 or DIN8
        }
        // Note: MOTOR_1 and MOTOR_3 don't have associated DIns based on the log pattern
        
        // Validate expected DIN
        if (expectedDinIndex >= 0) {
            if (!dinValues[expectedDinIndex]) {
                ESP_LOGE(TAG, "DIN%d FAILED: Expected HIGH for Motor_%d %s, got LOW", 
                         expectedDinIndex + 1, activeMotorIndex + 1, label);
                allPassed = false;
            }
        }
        
        // Check that all other DIns are LOW
        for (int i = 0; i < 8; i++) {
            if (i != expectedDinIndex && dinValues[i]) {
                ESP_LOGE(TAG, "DIN%d FAILED: Expected LOW, got HIGH", i + 1);
                allPassed = false;
            }
        }
    }
    
    return allPassed;
}

void setup(void)
{
    esp_log_level_set("*", ESP_LOG_DEBUG);

    // Array of motors for sequential operation
    const MotorNum_t motors[] = {MOTOR_1, MOTOR_2, MOTOR_3, MOTOR_4};
    bool allTestsPassed = true;

    delay(100);

    // Run each motor sequentially forward then backward
    for (int i = 0; i < 4; i++) {
        float percentage = 70.0f;//% of max speed
        dc.run(motors[i], FORWARD, percentage);
        // delay(100);
        bool currentOk = readAndLogCurrents("FORWARD", i, percentage);
        bool dinOk = checkAndPrintDInValues("FORWARD", i);
        allTestsPassed &= (currentOk && dinOk);
        // delay(900);

        percentage = 50.0f;//% of max speed
        dc.run(motors[i], REVERSE, percentage);
        // delay(100);
        currentOk = readAndLogCurrents("REVERSE", i, percentage);
        dinOk = checkAndPrintDInValues("REVERSE", i);
        allTestsPassed &= (currentOk && dinOk);
        // delay(900);

        //stop motor
        dc.stop(motors[i]);
        // delay(100);
        currentOk = readAndLogCurrents("STOPPED", i);
        dinOk = checkAndPrintDInValues("STOPPED", i);
        allTestsPassed &= (currentOk && dinOk);
        
        // Yield to allow IDLE task to run and feed watchdog
        //delay(100);
    }

    // check if H bridge fault is raised
    for (int i = MOTOR_1; i < MOTOR_MAX; i++) {
        uint8_t fault = dc.getFault((MotorNum_t)i);
        if (fault != 0) {
            ESP_LOGE(TAG, "Motor_%d DRV8873 Fault detected: 0x%02X", i + 1, fault);
            allTestsPassed = false;
        } else {
            ESP_LOGI(TAG, "Motor_%d DRV8873 No Fault detected", i + 1);
        }
    }

    // Final test summary
    if (allTestsPassed) {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "ALL TESTS PASSED");
        ESP_LOGI(TAG, "========================================");
    } else {
        ESP_LOGE(TAG, "========================================");
        ESP_LOGE(TAG, "SOME TESTS FAILED - CHECK LOGS ABOVE");
        ESP_LOGE(TAG, "========================================");
    }
}

void loop(void)
{
    delay(1000);
}
