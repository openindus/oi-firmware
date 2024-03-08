/**
 * @file AnalogOutput.h
 * @brief Analog Output
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "ad5413.h"

typedef enum {
    AOUT_1 = 0,
    AOUT_2 = 1,
    AOUT_MAX
} AnalogOutput_Num_t;

typedef enum {
    AOUT_MODE_M10V5_10V5 = 0,
    AOUT_MODE_0mA_20mA = 1,
    AOUT_MODE_UNDEFINED
} AnalogOutput_Mode_t;

class AnalogOutput
{
public:

    AnalogOutput(int num) : 
        _num(num),
        _mode(AOUT_MODE_UNDEFINED) {}

    virtual int setMode(AnalogOutput_Mode_t mode) = 0;
    virtual int write(float value) = 0;

protected:

    int _num;
    AnalogOutput_Mode_t _mode;

};

class AnalogOutputAD5413 : public AnalogOutput
{
public:

    AnalogOutputAD5413(int num) :
        AnalogOutput(num) {}

    static int init(ad5413_config_t* config, int nb);

    int setMode(AnalogOutput_Mode_t mode);
    int write(float value);

private:

    static ad5413_device_t** _device;
    static bool _deviceInitialized;
    
};