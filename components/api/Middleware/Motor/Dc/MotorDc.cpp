/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file MotorDc.cpp
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "MotorDc.h"

std::vector<MotorDC_Config_t> MotorDc::_motorsConfig;
gpio_num_t MotorDc::_faultPin;

int MotorDc::init(std::vector<MotorDC_Config_t> motorsConfig, gpio_num_t faultPin)
{    
    int err = 0;

    /* Save config */
    _faultPin = faultPin;
    _motorsConfig = motorsConfig;

    /* Init fault pin */
    gpio_config_t cfg;
        
    /* Configure Cmd pin */
    cfg.pin_bit_mask = (1ULL << _faultPin);
    cfg.mode = GPIO_MODE_INPUT;
    cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;

    err |= gpio_config(&cfg);

    /* Init motors pins */
    for(auto motorConfig = _motorsConfig.begin(); motorConfig != _motorsConfig.end(); motorConfig++) {
        /* Configure IN1 & IN2 and DISABLE pins */
        cfg.pin_bit_mask = (1ULL << motorConfig->in1_pwm) | (1ULL << motorConfig->in2_pwm) | (1ULL << motorConfig->disable);
        cfg.mode = GPIO_MODE_OUTPUT;
        cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
        cfg.intr_type = GPIO_INTR_DISABLE;
        err |= gpio_config(&cfg);

        err |= gpio_set_level(motorConfig->in1_pwm, 0);
        err |= gpio_set_level(motorConfig->in2_pwm, 0);
        err |= gpio_set_level(motorConfig->disable, 1);
    }

    return err;
}

void MotorDc::run(MotorNum_t motor, MotorDirection_t direction)
{
    gpio_set_level(_motorsConfig.at(motor).disable, 0);

    if (direction == FORWARD) {
        gpio_set_level(_motorsConfig.at(motor).in1_pwm, 1);
        gpio_set_level(_motorsConfig.at(motor).in2_pwm, 0);
    } else {
        gpio_set_level(_motorsConfig.at(motor).in1_pwm, 0);
        gpio_set_level(_motorsConfig.at(motor).in2_pwm, 1);
    }
}

void MotorDc::stop(MotorNum_t motor)
{
    gpio_set_level(_motorsConfig.at(motor).in1_pwm, 0);
    gpio_set_level(_motorsConfig.at(motor).in2_pwm, 0);
    gpio_set_level(_motorsConfig.at(motor).disable, 1);
}