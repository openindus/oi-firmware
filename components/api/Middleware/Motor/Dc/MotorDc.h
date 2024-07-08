/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorDc.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */


#pragma once

#include "Global.h"
#include "Motor.h"
#include "DigitalInputs.h"
#include "driver/gpio.h"

typedef struct {
    gpio_num_t in1_pwm;
    gpio_num_t in2_pwm;
    gpio_num_t disable;
} MotorDC_Config_t;

class MotorDc : public Motor
{
public:

    static int init(std::vector<MotorDC_Config_t> motorsConfig, gpio_num_t faultPin);
    
    static void run(MotorNum_t motor, MotorDirection_t direction);

    static void stop(MotorNum_t motor); 

private:

    static std::vector<MotorDC_Config_t> _motorsConfig;
    static gpio_num_t _faultPin;
};