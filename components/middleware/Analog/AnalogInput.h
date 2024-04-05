/**
 * @file AnalogInput.h
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
    AIN_VOLTAGE_RANGE_0_10V24 = ADS866X_VOLTAGE_RANGE_5,
    AIN_VOLTAGE_RANGE_0_5V12 = ADS866X_VOLTAGE_RANGE_6,
    AIN_VOLTAGE_RANGE_0_2V56 = ADS866X_VOLTAGE_RANGE_7,
    AIN_VOLTAGE_RANGE_0_1V28 = ADS866X_VOLTAGE_RANGE_8,
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

typedef struct {
    AnalogInput_Mode_t mode[AIN_MAX];
    gpio_num_t modePin[AIN_MAX];
    AnalogInput_VoltageRange_t voltageRange[AIN_MAX];
    ads866x_config_t* ads866xConfig;
} AnalogInputAds866xConfig_t;


class AnalogInput
{
public:

    AnalogInput(AnalogInput_Num_t num) : _num(num) {}

    virtual int read(void) = 0;
    virtual void setVoltageRange(AnalogInput_VoltageRange_t range) = 0;
    virtual uint8_t getVoltageRange(void) = 0;

protected:

    AnalogInput_Num_t _num;
    static AnalogInput_VoltageRange_t _voltage_range[AIN_MAX];
};

class AnalogInputAds866x : public AnalogInput
{
public:

    AnalogInputAds866x(AnalogInput_Num_t num) : AnalogInput(num) {}

    static int init(AnalogInputAds866xConfig_t* config);

    int read(void) override;
    float read(AnalogInput_Unit_t unit);
    void setMode(AnalogInput_Mode_t mode);
    uint8_t getMode(void);
    void setVoltageRange(AnalogInput_VoltageRange_t range) override;
    uint8_t getVoltageRange(void) override;

private:

    static gpio_num_t _modePin[AIN_MAX];
    static AnalogInput_Mode_t _mode[AIN_MAX];

    static bool _deviceInitialized;
};