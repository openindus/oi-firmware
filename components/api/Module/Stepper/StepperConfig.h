/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperConfig.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include "StepperPinout.h"

#if defined(CONFIG_STEPPER)

#define STEPPER_CONFIG_MOTOR_DEFAULT()                          \
{                                                               \
    .spi_host = STEPPER_SPI_HOST,                               \
    .spi_freq = STEPPER_SPI_FREQ,                               \
    .spi_pin_mosi = STEPPER_SPI_PIN_MOSI,                       \
    .spi_pin_miso = STEPPER_SPI_PIN_MISO,                       \
    .spi_pin_clk = STEPPER_SPI_PIN_CLK,                         \
    .spi_pin_cs = STEPPER_SPI_PIN_CS,                           \
    .pin_busy_sync = STEPPER_GPIO_PIN_BUSY_SYNC,                \
    .pin_flag = STEPPER_GPIO_PIN_FLAG,                          \
    .num_of_devices = 2,                                        \
    .pin_sw = {STEPPER_GPIO_PIN_D1_SW},                         \
    .pin_stby_rst = {STEPPER_GPIO_PIN_D1_STBY_RST},             \
    .pwm_pin_stck = STEPPER_PWM_PIN_STCK,                       \
    .pwm_timer = STEPPER_PWM_TIMER,                             \
    .pwm_mode = STEPPER_PWM_MODE,                               \
    .pwm_channel = STEPPER_PWM_CHANNEL                          \
}

#define STEPPER_CONFIG_DIN_DEFAULT()                           \
{                                                               \
    .pin_bit_mask = ((1ULL<<STEPPER_GPIO_PIN_DIN_1) |          \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_2) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_3) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_4)),           \
    .mode = GPIO_MODE_INPUT,                                    \
    .pull_up_en = GPIO_PULLUP_DISABLE,                          \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,                      \
    .intr_type = GPIO_INTR_ANYEDGE,                             \
}

#elif defined(CONFIG_STEPPER_VE)

#define STEPPER_CONFIG_MOTOR_DEFAULT()                          \
{                                                               \
    .spi_host = STEPPER_SPI_HOST,                               \
    .spi_freq = STEPPER_SPI_FREQ,                               \
    .spi_pin_mosi = STEPPER_SPI_PIN_MOSI,                       \
    .spi_pin_miso = STEPPER_SPI_PIN_MISO,                       \
    .spi_pin_clk = STEPPER_SPI_PIN_CLK,                         \
    .spi_pin_cs = STEPPER_SPI_PIN_CS,                           \
    .pin_busy_sync = STEPPER_GPIO_PIN_BUSY_SYNC,                \
    .pin_flag = STEPPER_GPIO_PIN_FLAG,                          \
    .num_of_devices = 1,                                        \
    .pin_sw = {STEPPER_GPIO_PIN_D1_SW},                         \
    .pin_stby_rst = {STEPPER_GPIO_PIN_D1_STBY_RST},             \
    .pwm_pin_stck = STEPPER_PWM_PIN_STCK,                       \
    .pwm_timer = STEPPER_PWM_TIMER,                             \
    .pwm_mode = STEPPER_PWM_MODE,                               \
    .pwm_channel = STEPPER_PWM_CHANNEL,                         \
}

#define STEPPER_CONFIG_DIN_DEFAULT()                           \
{                                                               \
    .pin_bit_mask = ((1ULL<<STEPPER_GPIO_PIN_DIN_1) |          \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_2) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_3) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_4) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_5) |           \
                    (1ULL<<STEPPER_GPIO_PIN_DIN_6)),           \
    .mode = GPIO_MODE_INPUT,                                    \
    .pull_up_en = GPIO_PULLUP_DISABLE,                          \
    .pull_down_en = GPIO_PULLDOWN_DISABLE,                      \
    .intr_type = GPIO_INTR_ANYEDGE,                             \
}

#endif