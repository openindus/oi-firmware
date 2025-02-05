/**
 * @file MotorDc.cpp
 * @brief MotorDc class implementation
 * @author 
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "MotorDc.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_MAX           (8191) // Duty cycle max. ((2 ** 13) - 1)
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

std::vector<MotorDC_PinConfig_t> MotorDc::_motorsConfig;
gpio_num_t MotorDc::_faultPin;

int MotorDc::init(std::vector<MotorDC_PinConfig_t> motorsConfig, gpio_num_t faultPin)
{    
    int err = 0;

    // Save config
    _faultPin = faultPin;
    _motorsConfig = motorsConfig;

    // Configure fault pin
    gpio_config_t input_conf;
    input_conf.pin_bit_mask = (1ULL << _faultPin);
    input_conf.mode = GPIO_MODE_INPUT;
    input_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    input_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    input_conf.intr_type = GPIO_INTR_DISABLE;
    err |= gpio_config(&input_conf);

    // Configure LEDC timer to generate PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    err |= ledc_timer_config(&ledc_timer);

    /* Configure all outputs */
    for(auto motorConfig = _motorsConfig.begin(); motorConfig != _motorsConfig.end(); motorConfig++) {
        // Configure disable pin
        gpio_config_t output_conf;
        output_conf.pin_bit_mask = (1ULL << motorConfig->disable);
        output_conf.mode = GPIO_MODE_OUTPUT;
        output_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        output_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        output_conf.intr_type = GPIO_INTR_DISABLE;
        err |= gpio_config(&output_conf);

        // Configure LEDC PWM channel
        ledc_channel_config_t ledc_channel;
        ledc_channel.speed_mode = LEDC_MODE;
        ledc_channel.intr_type = LEDC_INTR_DISABLE;
        ledc_channel.timer_sel = LEDC_TIMER;
        ledc_channel.duty = 0;
        ledc_channel.hpoint = 0;

        // Config IN1
        ledc_channel.gpio_num = motorConfig->in1.gpio;
        ledc_channel.channel = motorConfig->in1.channel;
        err |= ledc_channel_config(&ledc_channel);

        // Config IN2
        ledc_channel.gpio_num = motorConfig->in2.gpio;
        ledc_channel.channel = motorConfig->in2.channel;
        err |= ledc_channel_config(&ledc_channel);
    }

    return err;
}

void MotorDc::run(MotorNum_t motor, MotorDirection_t direction, float dutyCycle)
{
    // Check duty cycle
    if (dutyCycle < 0) dutyCycle = 0;
    if (dutyCycle > 100) dutyCycle = 100;

    // Enable motor
    gpio_set_level(_motorsConfig.at(motor).disable, 0);

    if (direction == FORWARD) {
        // IN1
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, LEDC_DUTY_MAX);
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);
        // IN2
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, 
            (uint32_t)(((100 - dutyCycle) * LEDC_DUTY_MAX) / 100));
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);
    } else {
        // IN1
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, 
            (uint32_t)(((100 - dutyCycle) * LEDC_DUTY_MAX) / 100));
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);
        // IN2
        ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, LEDC_DUTY_MAX);
        ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);
    }
}

void MotorDc::stop(MotorNum_t motor)
{
    // IN1
    ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel, 0);
    ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in1.channel);

    // IN2
    ledc_set_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel, 0);
    ledc_update_duty(LEDC_MODE, _motorsConfig.at(motor).in2.channel);

    // Disable motor
    gpio_set_level(_motorsConfig.at(motor).disable, 1);
}