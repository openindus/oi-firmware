/**
 * @file MotorDc.h
 * @brief MotorDc class definition
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "Global.h"
#include "Motor.h"
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

class MotorDc : public Motor
{
private:

    static std::vector<MotorDC_PinConfig_t> _motorsConfig;
    static gpio_num_t _faultPin;

protected:

    /**
     * @brief 
     * 
     * @param motorsConfig 
     * @param faultPin 
     * @return int 
     */
    static int init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin);

public:

    /**
     * @brief 
     * 
     * @param motor 
     * @param direction 
     * @param dutyCycle 
     */
    static void run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle);

    /**
     * @brief 
     * 
     * @param motor 
     */
    static void stop(MotorNum_t motor); 
};;