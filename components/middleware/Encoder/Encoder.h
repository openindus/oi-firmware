/**
 * @file Encoder.h
 * @brief Encoder class definition
 * @author Kévin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "DigitalInputs.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_log.h"
#include <map>
#include <math.h>
#include <stdio.h>

#define ENCODER_NB_INSTANCE_MAX PCNT_UNIT_MAX

class Encoder
{
public:
    Encoder(int index, std::map<DIn_Num_t, gpio_num_t> dinGpioMap)
        : _ppr(0), _revolutionCnt(0), _index(index), _speed(0), _dinGpioMap(dinGpioMap)
    {
        _pcntUnit = static_cast<pcnt_unit_t>((int)PCNT_UNIT_0 + index);
    }

    ~Encoder() {}

    int begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr);
    void end(void);

    void reset(void);
    int getRevolutions(void);
    int getPulses(void);
    float getAngle(void);
    float getSpeed(void);

private:
    int16_t _ppr;
    int _revolutionCnt;
    int _index;
    float _speed;

    std::map<DIn_Num_t, gpio_num_t> _dinGpioMap;
    pcnt_unit_t _pcntUnit;

    static void _pcntIsrHandler(void *arg);

    static void _speedTask(void *pvParameter);
};
