/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file OIType.h
 * @brief 
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#pragma once

#include <stdint.h>

#define SECONDS_FROM_1970_TO_2000 946684800

/* Common defines */
#define ADC_NB_SAMPLE 64
#define ADC_DEFAULT_VREF 1100
#define STOR_SENSOR_COEFF_K 1800
#define VSENSE_RESISTOR 1050

/**
 * @brief Available ETOR input
 *
 */
typedef enum
{
    ETOR1,
    ETOR2,
    ETOR3,
    ETOR4,
    ETOR5,
    ETOR6,
    ETOR7,
    ETOR8,
    ETOR9,
    ETOR10
} Etor_t;

/**
 * @brief Available STOR output
 *
 */
typedef enum
{
    STOR1,
    STOR2,
    STOR3,
    STOR4,
    STOR5,
    STOR6,
    STOR7,
    STOR8,
    STOR9,
    STOR10
} Stor_t;

/**
 * @brief Available analog input
 *
 */
typedef enum
{
    EANA1,
    EANA2,
    EANA3,
    EANA4,
    EANA5,
    EANA6,
    EANA7,
    EANA8,
    EANA9,
    EANA10
} Eana_t;


/**
 * @brief Available analog output
 *
 */
typedef enum
{
    SANA1,
    SANA2,
} Sana_t;

typedef enum
{
    RELAY1,
    RELAY2,
    RELAY3,
    RELAY4,
    RELAY5,
    RELAY6,
} Relay_t;

/**
 * @brief Interrupt mode for ETOR intputs
 *
 */
typedef enum
{
    NONE_MODE,
    RISING_MODE,
    FALLING_MODE,
    CHANGE_MODE
} InterruptMode_t;

/**
 * @brief Logic level
 * 
 */
typedef enum
{
    LOW_LEVEL,
    HIGH_LEVEL
} Level_t;

/**
 * @brief STOR modes
 *
 */
typedef enum
{
    STOR,
    PWM
} StorMode_t;

/* ADC Types */

typedef enum 
{
	UNDEF_MODE = 0xFF,
	CURRENT_MODE = 0,
	VOLTAGE_MODE = 1,
} AdcMode_t;

typedef enum 
{
	ADC_RES_10BITS = 10,
	ADC_RES_12BITS = 12
} AdcRes_t;

typedef enum
{
    DAC_ANALOG_RANGE_0_5_V      = 0,
    DAC_ANALOG_RANGE_0_10_V     = 1,
    DAC_ANALOG_RANGE_N5_5_V     = 2,
    DAC_ANALOG_RANGE_N10_10_V   = 3,
    UNDEF_ANALOG_MODE           = 0xFF
} DACAnalogMode_t;

typedef enum 
{
    DAC_CURRENT_RANGE_4_20_MA   = 5,
    DAC_CURRENT_RANGE_0_20_MA   = 6,
    DAC_CURRENT_RANGE_0_24_MA   = 7,
    UNDEF_CURRENT_MODE          = 0xFF
} DACCurrentMode_t;

typedef enum
{
    UNITS_RAW           = 0,
    UNITS_MILLIVOLTS,
    UNITS_MILLIAMPS,
} Units_t;

class DateTime
{
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const DateTime& copy);
    DateTime (const char* date, const char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const; 

protected:
    uint8_t yOff;
    uint8_t m;
    uint8_t d;
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
};