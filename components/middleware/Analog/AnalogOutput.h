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

    AnalogOutput() : 
        _mode(AOUT_MODE_M10V5_10V5),
        _isInitialized(false) {}

    virtual int init(void) = 0;

    virtual int setMode(AnalogOutput_Mode_t mode) = 0;
    virtual int write(float value) = 0;

protected:

    AnalogOutput_Mode_t _mode;
    bool _isInitialized;
    
};

class AnalogOutputAD5413 : public AnalogOutput
{
public:

    AnalogOutputAD5413(ad5413_config_t* config) :
        AnalogOutput(),
        _device(NULL),
        _config(config) {}

    int init(void);

    int setMode(AnalogOutput_Mode_t mode);
    int write(float value);

private:

    ad5413_device_t* _device;
    ad5413_config_t* _config;
    
};