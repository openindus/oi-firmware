/**
 * @file StepperPinout.h
 * @brief Stepper pinout
 * @author  Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(CONFIG_OI_STEPPER)

#if defined(CONFIG_IDF_TARGET_ESP32S3)

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

#elif defined(CONFIG_IDF_TARGET_ESP32S2)

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

#endif

#elif defined(CONFIG_OI_STEPPER_VE)

#if defined(CONFIG_IDF_TARGET_ESP32S2)

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

#endif
#endif