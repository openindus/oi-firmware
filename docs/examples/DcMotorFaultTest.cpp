/**
 * @file DcMotorFaultTest.cpp 
 * @author Georges de Massol
 * @brief DC Motor Fault Testing Example for OI-DC (DC Motor Controller) module
 * 
 * This example demonstrates fault detection and testing for the OpenIndus DC Motor Controller:
 * - Create intentional faults through rapid direction changes
 * - Monitor and detect DRV8873 faults
 * - Clear faults after detection
 * 
 * Hardware Wiring Diagram:
 * 
 *     OI-DC Module Pinout & Test Configuration
 *     =========================================
 * 
 *     Power Supply (24V)   
 *          +                                                         +----------------
 *          |                                                         |               |
 *          |                                                         |               |     
 *     +----|-------------------------+-------+--------------+--------+----+          |
 *     |    |        |       |        |       |      |       |        |    |          |
 *     |  9V-30V   9V-30V   /B2      /A2      |     /B1     /A1       |    |        _____
 *     |   VIN      VIN     DIN4    HB4_2   HB3_2   DIN2   HB2_2   HB1_2   |       /     \
 *     |                                                                   |      |   M   |
 *     |                                                                   |       \_____/       
 *     |   GND      GND     DIN3    HB4_1   HB3_1   DIN1   HB2_1   HB1_1   |          |
 *     |    |        |       B2       A2      |      B1      A1       |    |          |
 *     |    |        |       |        |       |      |       |        |    |          |
 *     +----+-------------------------+-------+--------------+--------+----+          |
 *          |                                                         |               |
 *          |                                                         |               |
 *          -                                                         +----------------
 *         GND
 * 
 * Expected Results:
 * -----------------
 * - When running MOTOR_1 with rapid direction changes, DRV8873 fault should be detected
 * - Expected fault code: 0x40 (overcurrent protection)
 * - Other motors should not show faults
 * 
 * @date November 2025
 */

#include "OpenIndus.h"
#include "Arduino.h"

Dc dc;  // Create DC motor controller object

static const char TAG[] = "Main";

void playMelody(bool allTestsPassed)
{
    // Play a melody by toggling the motor at note frequencies
    struct Note { int freq; int durationMs; };
    const Note successMelody[] = {
        {392, 120}, // G4
        {523, 120}, // C5
        {659, 120}, // E5
        {784, 240}, // G5 (long)
        {659, 120}, // E5
        {784, 480}, // G5 (long)
    };
    const Note failureMelody[] = {
        {392, 180}, // G4
        {349, 180}, // F4
        {330, 180}, // E4
        {294, 180}, // D4 (long)
        {0, 90},   // Rest
        {294, 180}, // D4
        {262, 400}, // C4 (long)
    };
    const Note* melody;
    int melodyLen;
    if (allTestsPassed) {
        melody = successMelody;
        melodyLen = sizeof(successMelody) / sizeof(successMelody[0]);
    } else {
        melody = failureMelody;
        melodyLen = sizeof(failureMelody) / sizeof(failureMelody[0]);
    }

    for (int n = 0; n < melodyLen; n++) {
        if (melody[n].freq > 0) {
            int period_us = 1000000 / melody[n].freq;
            int cycles = (melody[n].durationMs * 1000) / period_us;
            for (int i = 0; i < cycles; i++) {
                dc.run(MOTOR_1, FORWARD, 100.0f);
                delayMicroseconds(period_us / 2);
                dc.run(MOTOR_1, REVERSE, 100.0f);
                delayMicroseconds(period_us / 2);
            }
        } else {
            // Rest (no sound)
            delay(melody[n].durationMs);
        }
        dc.stop(MOTOR_1);
        delay(30);
    }
}

void setup(void)
{
    bool allTestsPassed = true;
    
    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "Starting DC Motor Fault Test");
    ESP_LOGI(TAG, "========================================");
    
    // First check initial fault status for all motors
    ESP_LOGI(TAG, "Checking initial fault status...");
    for (int i = MOTOR_1; i < MOTOR_MAX; i++) {
        uint8_t fault = dc.getFault((MotorNum_t)i);
        if (fault != 0) {
            ESP_LOGW(TAG, "Motor_%d DRV8873 Fault detected at startup: 0x%02X", i + 1, fault);
            dc.clearFault((MotorNum_t)i);
        } else {
            ESP_LOGI(TAG, "Motor_%d DRV8873 No Fault detected", i + 1);
        }
    }
    
    // Creating a fault by running MOTOR_1 forward and reverse quickly
    ESP_LOGI(TAG, "Creating a current fault on MOTOR_1 by rapid direction changes");
    ESP_LOGI(TAG, "This should trigger DRV8873 overcurrent protection (fault code 0x40)");
    
    for (int j = 0; j < 200; j++) {
        dc.run(MOTOR_1, FORWARD, 100.0f);
        delay(4);
        dc.run(MOTOR_1, REVERSE, 100.0f);
        delay(4);
    }
    dc.brake(MOTOR_1);

    // Wait 10ms for stabilization
    vTaskDelay(pdMS_TO_TICKS(10));

    // Read and log fault status for all devices
    ESP_LOGI(TAG, "Checking fault status after stress test...");
    for (int i = MOTOR_1; i < MOTOR_MAX; i++) {
        uint8_t fault = dc.getFault((MotorNum_t)i);
        if ((fault == 0x40 && i == MOTOR_1) || (fault == 0 && i != MOTOR_1)) {
            ESP_LOGI(TAG, "Motor_%d DRV8873 Fault (expected) :0x%02X", i + 1, fault);
        } else {
            ESP_LOGE(TAG, "Motor_%d DRV8873 Fault NOT detected when expected!", i + 1);
            allTestsPassed = false;
        }
        // Clear any existing faults
        dc.clearFault((MotorNum_t)i);
    }
    
    // Final test summary
    if (allTestsPassed) {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "FAULT TEST PASSED");
        ESP_LOGI(TAG, "Motor_1 correctly shows fault 0x40");
        ESP_LOGI(TAG, "Other motors show no faults");
        ESP_LOGI(TAG, "========================================");
    } else {
        ESP_LOGE(TAG, "========================================");
        ESP_LOGE(TAG, "FAULT TEST FAILED");
        ESP_LOGE(TAG, "Check fault detection logic");
        ESP_LOGE(TAG, "========================================");
    }
    playMelody(allTestsPassed);
}

void loop(void)
{
    delay(5000);
}
