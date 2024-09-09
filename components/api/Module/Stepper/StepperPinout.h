/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file StepperPinout.h
 * @brief Functions for stepper module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_STEPPER)

#if defined(CONFIG_IDF_TARGET_ESP32S3)

/* Config */
#define STEPPER_MOTOR_NB    2
#define STEPPER_DIN_NUM     4

/* SPI pins */
#define STEPPER_SPI_PIN_MOSI             GPIO_NUM_48
#define STEPPER_SPI_PIN_MISO             GPIO_NUM_18
#define STEPPER_SPI_PIN_CLK              GPIO_NUM_15
#define STEPPER_SPI_PIN_CS               GPIO_NUM_17

/* SPI Device Host */
#define STEPPER_SPI_HOST                 SPI2_HOST
#define STEPPER_SPI_FREQ                 SPI_MASTER_FREQ_8M

/* DIN */
#define STEPPER_GPIO_PIN_DIN_1          GPIO_NUM_11
#define STEPPER_GPIO_PIN_DIN_2          GPIO_NUM_12
#define STEPPER_GPIO_PIN_DIN_3          GPIO_NUM_13
#define STEPPER_GPIO_PIN_DIN_4          GPIO_NUM_10

/* Device 1 GPIOs pins */
#define STEPPER_GPIO_PIN_D1_SW           GPIO_NUM_34
#define STEPPER_GPIO_PIN_D1_STBY_RST     GPIO_NUM_36
#define STEPPER_GPIO_PIN_D1_BUSY_SYNC    GPIO_NUM_35
#define STEPPER_GPIO_PIN_D1_FLAG         GPIO_NUM_37
#define STEPPER_PWM_PIN_D1_STCK          GPIO_NUM_33
#define STEPPER_PWM_TIMER_D1             LEDC_TIMER_1
#define STEPPER_PWM_CHANNEL_D1           LEDC_CHANNEL_0

/* Device 2 GPIOs pins */
#define STEPPER_GPIO_PIN_D2_SW           GPIO_NUM_47
#define STEPPER_GPIO_PIN_D2_STBY_RST     GPIO_NUM_19
#define STEPPER_GPIO_PIN_D2_BUSY_SYNC    GPIO_NUM_20
#define STEPPER_GPIO_PIN_D2_FLAG         GPIO_NUM_26
#define STEPPER_PWM_PIN_D2_STCK          GPIO_NUM_21
#define STEPPER_PWM_TIMER_D2             LEDC_TIMER_2
#define STEPPER_PWM_CHANNEL_D2           LEDC_CHANNEL_1

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

/* Config */
#define STEPPER_MOTOR_NB    2
#define STEPPER_DIN_NUM     4

/* SPI pins */
#define STEPPER_SPI_PIN_MOSI             GPIO_NUM_37
#define STEPPER_SPI_PIN_MISO             GPIO_NUM_35
#define STEPPER_SPI_PIN_CLK              GPIO_NUM_36
#define STEPPER_SPI_PIN_CS               GPIO_NUM_34

/* SPI Device Host */
#define STEPPER_SPI_HOST                 SPI2_HOST
#define STEPPER_SPI_FREQ                 SPI_MASTER_FREQ_8M

/* DIN */
#define STEPPER_GPIO_PIN_DIN_1          GPIO_NUM_41
#define STEPPER_GPIO_PIN_DIN_2          GPIO_NUM_40
#define STEPPER_GPIO_PIN_DIN_3          GPIO_NUM_39
#define STEPPER_GPIO_PIN_DIN_4          GPIO_NUM_38

/* Device 1 GPIOs pins */
#define STEPPER_GPIO_PIN_D1_SW           GPIO_NUM_42
#define STEPPER_GPIO_PIN_D1_STBY_RST     GPIO_NUM_11
#define STEPPER_GPIO_PIN_D1_BUSY_SYNC    GPIO_NUM_12
#define STEPPER_GPIO_PIN_D1_FLAG         GPIO_NUM_14
#define STEPPER_PWM_PIN_D1_STCK          GPIO_NUM_13
#define STEPPER_PWM_TIMER_D1             LEDC_TIMER_1
#define STEPPER_PWM_CHANNEL_D1           LEDC_CHANNEL_0

/* Device 2 GPIOs pins */
#define STEPPER_GPIO_PIN_D2_SW           GPIO_NUM_15
#define STEPPER_GPIO_PIN_D2_STBY_RST     GPIO_NUM_16
#define STEPPER_GPIO_PIN_D2_BUSY_SYNC    GPIO_NUM_12
#define STEPPER_GPIO_PIN_D2_FLAG         GPIO_NUM_14
#define STEPPER_PWM_PIN_D2_STCK          GPIO_NUM_13
#define STEPPER_PWM_TIMER_D2             LEDC_TIMER_1
#define STEPPER_PWM_CHANNEL_D2           LEDC_CHANNEL_0

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

/* Config */
#define STEPPER_MOTOR_NB   1
#define STEPPER_DIN_NUM     6

/* SPI pins */
#define STEPPER_SPI_PIN_MOSI             GPIO_NUM_35
#define STEPPER_SPI_PIN_MISO             GPIO_NUM_37
#define STEPPER_SPI_PIN_CLK              GPIO_NUM_36
#define STEPPER_SPI_PIN_CS               GPIO_NUM_34

/* SPI Device Host */
#define STEPPER_SPI_HOST                 SPI2_HOST
#define STEPPER_SPI_FREQ                 SPI_MASTER_FREQ_8M

/* DIN */
#define STEPPER_GPIO_PIN_DIN_1          GPIO_NUM_13
#define STEPPER_GPIO_PIN_DIN_2          GPIO_NUM_6
#define STEPPER_GPIO_PIN_DIN_3          GPIO_NUM_8
#define STEPPER_GPIO_PIN_DIN_4          GPIO_NUM_12
#define STEPPER_GPIO_PIN_DIN_5          GPIO_NUM_11
#define STEPPER_GPIO_PIN_DIN_6          GPIO_NUM_9

/* DOUT */
#define STEPPER_GPIO_PIN_DOUT_1          GPIO_NUM_15
#define STEPPER_GPIO_PIN_DOUT_2          GPIO_NUM_16
#define STEPPER_GPIO_PIN_DOUT_1_STAT     GPIO_NUM_14
#define STEPPER_GPIO_PIN_DOUT_2_STAT     GPIO_NUM_45

/* Device 1 GPIOs pins */
#define STEPPER_GPIO_PIN_D1_SW           GPIO_NUM_19
#define STEPPER_GPIO_PIN_D1_STBY_RST     GPIO_NUM_20
#define STEPPER_GPIO_PIN_D1_BUSY_SYNC    GPIO_NUM_38
#define STEPPER_GPIO_PIN_D1_FLAG         GPIO_NUM_26
#define STEPPER_PWM_PIN_D1_STCK          GPIO_NUM_33
#define STEPPER_PWM_TIMER_D1             LEDC_TIMER_1
#define STEPPER_PWM_CHANNEL_D1           LEDC_CHANNEL_0

/* step clock */
#define STEPPER_PWM_TIMER                LEDC_TIMER_1
#define STEPPER_PWM_MODE                 LEDC_LOW_SPEED_MODE
#define STEPPER_PWM_CHANNEL              LEDC_CHANNEL_0

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
