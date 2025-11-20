/**
 * @file MotorDc.h
 * @brief MotorDc class definition
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Motor.h"
#include <vector>
#include "driver/gpio.h"
#include "driver/ledc.h"

typedef struct {
    struct {
        gpio_num_t gpio;
        ledc_channel_t channel; 
    } in1;
    struct {
        gpio_num_t gpio;
        ledc_channel_t channel; 
    } in2;
    gpio_num_t disable;
} MotorDC_PinConfig_t;

/**
 * @class MotorDc
 * @brief DC Motor control class
 */
class MotorDc : public Motor
{
public:

    /**
     * @brief Run a DC motor with specified direction and duty cycle
     * 
     * @param motor Motor number
     * @param direction Motor direction (FORWARD/REVERSE)
     * @param dutyCycle Duty cycle percentage (0..100)
     */
    static void run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle);

    /**
     * @brief Stop a DC motor
     * 
     * @param motor Motor number
     */
    static void stop(MotorNum_t motor);

    /**
     * @brief Get current consumption of a DC motor
     * 
     * @param motor Motor number
     * @return float current in A
     */
    static float getCurrent(MotorNum_t motor);

protected:
    static int init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin);
    static void initADC(void);
    static void initHBridge(void);
    static void initSPI(void);

private:
    static std::vector<MotorDC_PinConfig_t> _motorsConfig;
    static gpio_num_t _faultPin;
    static std::vector<MotorDirection_t> _directions;

    static int _registerCLI(void);
};
