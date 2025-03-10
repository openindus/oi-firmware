/**
 * @file StepperConfig.h
 * @brief Stepper configuration file
 * @author  Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_STEPPER)

#if defined(CONFIG_IDF_TARGET_ESP32S3)

#define STEPPER_MOTOR_NB    2
#define STEPPER_DIN_NB      4

#define STEPPER_CONFIG_MOTOR_DEFAULT()                                                  \
{                                                                                       \
    .num_of_devices = STEPPER_MOTOR_NB,                                                 \
    .spi_host = STEPPER_SPI_HOST,                                                       \
    .spi_freq = STEPPER_SPI_FREQ,                                                       \
    .spi_pin_mosi = STEPPER_SPI_PIN_MOSI,                                               \
    .spi_pin_miso = STEPPER_SPI_PIN_MISO,                                               \
    .spi_pin_clk = STEPPER_SPI_PIN_CLK,                                                 \
    .spi_pin_cs = STEPPER_SPI_PIN_CS,                                                   \
    .pin_busy_sync = {STEPPER_GPIO_PIN_D1_BUSY_SYNC, STEPPER_GPIO_PIN_D2_BUSY_SYNC},    \
    .pin_flag = {STEPPER_GPIO_PIN_D1_FLAG, STEPPER_GPIO_PIN_D2_FLAG},                   \
    .pin_sw = {STEPPER_GPIO_PIN_D1_SW, STEPPER_GPIO_PIN_D2_SW},                         \
    .pin_stby_rst = {STEPPER_GPIO_PIN_D1_STBY_RST, STEPPER_GPIO_PIN_D2_STBY_RST},       \
    .pwm_pin_stck = {STEPPER_PWM_PIN_D1_STCK, STEPPER_PWM_PIN_D2_STCK},                 \
    .pwm_timer = {STEPPER_PWM_TIMER_D1, STEPPER_PWM_TIMER_D2},                          \
    .pwm_channel = {STEPPER_PWM_CHANNEL_D1, STEPPER_PWM_CHANNEL_D2},                    \
}

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

#define STEPPER_MOTOR_NB    2
#define STEPPER_DIN_NB      4

#define STEPPER_CONFIG_MOTOR_DEFAULT()                                                  \
{                                                                                       \
    .num_of_devices = STEPPER_MOTOR_NB,                                                 \
    .spi_host = STEPPER_SPI_HOST,                                                       \
    .spi_freq = STEPPER_SPI_FREQ,                                                       \
    .spi_pin_mosi = STEPPER_SPI_PIN_MOSI,                                               \
    .spi_pin_miso = STEPPER_SPI_PIN_MISO,                                               \
    .spi_pin_clk = STEPPER_SPI_PIN_CLK,                                                 \
    .spi_pin_cs = STEPPER_SPI_PIN_CS,                                                   \
    .pin_busy_sync = {STEPPER_GPIO_PIN_D1_BUSY_SYNC, STEPPER_GPIO_PIN_D2_BUSY_SYNC},    \
    .pin_flag = {STEPPER_GPIO_PIN_D1_FLAG, STEPPER_GPIO_PIN_D2_FLAG},                   \
    .pin_sw = {STEPPER_GPIO_PIN_D1_SW, STEPPER_GPIO_PIN_D2_SW},                         \
    .pin_stby_rst = {STEPPER_GPIO_PIN_D1_STBY_RST, STEPPER_GPIO_PIN_D2_STBY_RST},       \
    .pwm_pin_stck = {STEPPER_PWM_PIN_D1_STCK, STEPPER_PWM_PIN_D2_STCK},                 \
    .pwm_timer = {STEPPER_PWM_TIMER_D1, STEPPER_PWM_TIMER_D2},                          \
    .pwm_channel = {STEPPER_PWM_CHANNEL_D1, STEPPER_PWM_CHANNEL_D2},                    \
}

#endif

#elif defined(OI_STEPPER_VE)

#if defined(CONFIG_IDF_TARGET_ESP32S2)

#define STEPPER_MOTOR_NB    1
#define STEPPER_DIN_NB      6

#define STEPPER_CONFIG_MOTOR_DEFAULT()                      \
{                                                           \
    .num_of_devices = STEPPER_MOTOR_NB,                     \
    .spi_host = STEPPER_SPI_HOST,                           \
    .spi_freq = STEPPER_SPI_FREQ,                           \
    .spi_pin_mosi = STEPPER_SPI_PIN_MOSI,                   \
    .spi_pin_miso = STEPPER_SPI_PIN_MISO,                   \
    .spi_pin_clk = STEPPER_SPI_PIN_CLK,                     \
    .spi_pin_cs = STEPPER_SPI_PIN_CS,                       \
    .pin_busy_sync = {STEPPER_GPIO_PIN_D1_BUSY_SYNC},       \
    .pin_flag = {STEPPER_GPIO_PIN_D1_FLAG},                 \
    .pin_sw = {STEPPER_GPIO_PIN_D1_SW},                     \
    .pin_stby_rst = {STEPPER_GPIO_PIN_D1_STBY_RST},         \
    .pwm_pin_stck = {STEPPER_PWM_PIN_D1_STCK},              \
    .pwm_timer = {STEPPER_PWM_TIMER_D1},                    \
    .pwm_channel = {STEPPER_PWM_CHANNEL_D1},                \
}

#endif

#endif 