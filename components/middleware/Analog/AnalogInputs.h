/**
 * @file AnalogInputs.h
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ads866x.h"

#define AIN_CURRENT_MODE_RES_VALUE (float)100.0

typedef enum {
    AIN_1 = 0,
    AIN_2 = 1,
    AIN_3 = 2,
    AIN_4 = 3,
    AIN_MAX
} AnalogInput_Num_t;

typedef enum {
    AIN_MODE_VOLTAGE = 0,
    AIN_MODE_CURRENT = 1,
    AIN_MODE_UNDEFINED
} AnalogInput_Mode_t;

typedef enum {
    AIN_VOLTAGE_RANGE_0_10V24 = 5,
    AIN_VOLTAGE_RANGE_0_5V12 = 6,
    AIN_VOLTAGE_RANGE_0_2V56 = 7,
    AIN_VOLTAGE_RANGE_0_1V28 = 8,
    AIN_VOLTAGE_RANGE_UNDEFINED
} AnalogInput_VoltageRange_t;

typedef enum {
    AIN_UNIT_RAW = 0,
    AIN_UNIT_MILLIVOLT = 1,
    AIN_UNIT_MILLIAMP = 2,
    AIN_UNIT_VOLT = 3,
    AIN_UNIT_AMP = 4,
    AIN_UNIT_UNDEFINED
} AnalogInput_Unit_t;

typedef enum {
    ANALOG_INPUT_ADS866X = 0,
    ANALOG_INPUT_MAX
} AnalogInputType_t;

class AnalogInputAds866x 
{
public:

    AnalogInputAds866x(uint8_t num, gpio_num_t cmdGpio);

    void init(AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode);

    int read(void);
    float read(AnalogInput_Unit_t unit);
    void setMode(AnalogInput_Mode_t mode);
    uint8_t getMode(void);
    void setVoltageRange(AnalogInput_VoltageRange_t range);
    uint8_t getVoltageRange(void);
    gpio_num_t getModePin();

private:

    int _num;
    gpio_num_t _modePin;
    AnalogInput_Mode_t _mode;
    AnalogInput_VoltageRange_t _voltage_range;
};

class AnalogInputs
{
public:

    AnalogInputs(const gpio_num_t* cmdGpio, uint8_t nb);

    int init(ads866x_config_t *ads866xConfig, AnalogInput_VoltageRange_t range, AnalogInput_Mode_t mode);

    int read(AnalogInput_Num_t num);
    float read(AnalogInput_Num_t num, AnalogInput_Unit_t unit);
    void setMode(AnalogInput_Num_t num, AnalogInput_Mode_t mode);
    uint8_t getMode(AnalogInput_Num_t num);
    void setVoltageRange(AnalogInput_Num_t num, AnalogInput_VoltageRange_t range);
    uint8_t getVoltageRange(AnalogInput_Num_t num);

private:

    uint8_t _nb;
    AnalogInputType_t _type;

    AnalogInputAds866x* _ains[AIN_MAX];
};