/**
 * @file DcMotorSimple.cpp 
 * @author Georges de Massol
 * @brief Simple example for OI-DC (DC Motor Controller) module
 * 
 * This example demonstrates basic motor control with the OpenIndus DC module:
 * - Run motor forward at full speed
 * - Measure current
 * - Run motor backward at full speed
 * - Measure current
 * - Stop motor
 * - Measure current (should be near 0A)
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
 * @date November 2025
 */

#include "OpenIndus.h"
#include "Arduino.h"

Dc dc;  // Create DC motor controller object

static const char TAG[] = "Main";

void setup(void)
{
}

void loop(void)
{
    float current;
    
    // 1. Run motor forward at 100% speed
    dc.run(MOTOR_1, FORWARD, 100);
    current = dc.getCurrent(MOTOR_1);
    ESP_LOGI(TAG, "MOTOR_1 FORWARD: Current = %.3f A", current);
    
    delay(2000);  // Run for 2 seconds
    
    // 2. Run motor backward at 100% speed
    dc.run(MOTOR_1, REVERSE, 100);
    current = dc.getCurrent(MOTOR_1);
    ESP_LOGI(TAG, "MOTOR_1 REVERSE: Current = %.3f A", current);
    
    delay(2000);  // Run for 2 seconds
    
    // 3. Stop motor
    dc.stop(MOTOR_1);
    current = dc.getCurrent(MOTOR_1);
    ESP_LOGI(TAG, "MOTOR_1 STOPPED: Current = %.3f A", current);
    // Nothing to do in loop
    delay(2000);
}