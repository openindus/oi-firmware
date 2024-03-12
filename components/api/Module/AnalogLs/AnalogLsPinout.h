/**
 * @file AnalogLsPinout.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_ANALOG_LS)

/* SPI */
#define ANALOG_LS_SPI_HOST          SPI2_HOST
#define ANALOG_LS_SPI_FREQ          SPI_MASTER_FREQ_8M
#define ANALOG_LS_SPI_PIN_MISO      GPIO_NUM_21
#define ANALOG_LS_SPI_PIN_MOSI      GPIO_NUM_20
#define ANALOG_LS_SPI_PIN_SCLK      GPIO_NUM_19

/* ADC */
#define ANALOG_LS_GPIO_PIN_CS       GPIO_NUM_47

#endif