/**
 * @file AnalogInput.h
 * @brief Analog Input
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ads866x/ads866x.h"

typedef enum {
    AIN_1 = 0,
    AIN_2 = 1,
    AIN_3 = 2,
    AIN_4 = 3,
    AIN_5 = 4,
    AIN_6 = 5,
    AIN_7 = 6,
    AIN_8 = 7,
    AIN_9 = 8,
    AIN_10 = 9,
    AIN_MAX
} AnalogInput_Num_t;

typedef enum {
    AIN_MODE_CURRENT = 0,
    AIN_MODE_VOLTAGE = 1,
    AIN_MODE_UNDEFINED
} AnalogInput_Mode_t;

typedef enum {
    AIN_RES_10_BITS = 0,
    AIN_RES_12_BITS = 1,
    AIN_RES_UNDEFINED
} AnalogInput_Resolution_t;

typedef enum {
    AIN_UNIT_RAW = 0,
    AIN_UNIT_MILLIVOLTS = 1,
    AIN_UNIT_MILLIAMPS = 2,
    AIN_UNIT_VOLTS = 3,
    AIN_UNIT_AMPS = 5,
    AIN_UNIT_UNDEFINED
} AnalogInput_Unit_t;

class AnalogInput
{
public:

    AnalogInput(AnalogInput_Num_t num) : 
        _num(num),
        _mode(AIN_MODE_UNDEFINED),
        _res(AIN_RES_UNDEFINED) {}

    virtual int read(void) = 0;
    virtual float read(AnalogInput_Unit_t unit) = 0;
    virtual void setMode(AnalogInput_Mode_t mode) = 0;
    virtual void setResolution(AnalogInput_Resolution_t res) = 0;
    virtual void setReference(float ref) = 0;

protected:

    AnalogInput_Num_t _num;
    AnalogInput_Mode_t _mode;
    AnalogInput_Resolution_t _res;
    
};

class AnalogInputAds866x : public AnalogInput
{
public:

    AnalogInputAds866x(AnalogInput_Num_t num) :
        AnalogInput(num) {}

    static int init(Ads866x_DeviceConfig_t* config);

    int read(void) override;
    float read(AnalogInput_Unit_t unit) override;
    void setMode(AnalogInput_Mode_t mode) override;
    void setResolution(AnalogInput_Resolution_t res) override;
    void setReference(float ref) override;

private:

    static bool _deviceInitialized;

};