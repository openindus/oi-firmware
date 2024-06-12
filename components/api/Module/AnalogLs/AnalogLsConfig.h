/**
 * @file AnalogLsConfig.h
 * @brief Analog LS
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#if defined(OI_ANALOG_LS)

/* --- Pinout --- */

/* ADC */
#define ANALOG_LS_ADC_SPI_HOST          SPI2_HOST
#define ANALOG_LS_ADC_SPI_FREQ          SPI_MASTER_FREQ_8M
#define ANALOG_LS_ADC_PIN_SPI_MISO      GPIO_NUM_18
#define ANALOG_LS_ADC_PIN_SPI_MOSI      GPIO_NUM_37
#define ANALOG_LS_ADC_PIN_SPI_SCLK      GPIO_NUM_15
#define ANALOG_LS_ADC_PIN_SPI_CS        GPIO_NUM_17
#define ANALOG_LS_ADC_PIN_START_SYNC    GPIO_NUM_47
#define ANALOG_LS_ADC_PIN_DRDY          GPIO_NUM_33
#define ANALOG_LS_ADC_PIN_RESET         GPIO_NUM_16

/* IO Mux */

/* Digipot - thermometer */

#endif