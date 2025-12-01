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
#include "driver/pulse_cnt.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include <map>
#include <math.h>
#include <stdio.h>

#define ENCODER_NB_INSTANCE_MAX SOC_PCNT_UNITS_PER_GROUP

class Encoder
{
public:
    Encoder(int index, std::map<DIn_Num_t, gpio_num_t> dinGpioMap): 
        _ppr(0), 
        _revolutionCnt(0),
        _index(index), 
        _speed(0), 
        _dinGpioMap(dinGpioMap), 
        _pcntUnit(nullptr) {}

    ~Encoder() {}

    /**
     * @brief Encoder initialization
     *
     * @param A Digital input number A
     * @param B Digital input number B
     * @param ppr Pulse per revolution
     * @return int
     */
    int begin(DIn_Num_t A, DIn_Num_t B, int16_t ppr);

    /**
     * @brief Encoder end
     *
     */
    void end(void);

    /**
     * @brief Reset the number of pulses and revolutions
     *
     */
    void reset(void);

    /**
     * @brief Get the number of revolutions
     *
     * @return int number of revolutions
     */
    int getRevolutions(void);

    /**
     * @brief Get the number of pulses
     *
     * @return int number of pulses
     */
    int getPulses(void);

    /**
     * @brief Get the angle
     *
     * @return float angle in radians
     */
    float getAngle(void);

    /**
     * @brief Get the speed
     *
     * @return float speed in pulses per second
     */
    float getSpeed(void);

private:
    int16_t _ppr;
    int _revolutionCnt;
    int _index;
    float _speed;

    std::map<DIn_Num_t, gpio_num_t> _dinGpioMap;
    pcnt_unit_handle_t _pcntUnit;

    /* PCNT ISR handler */
    static bool _pcntIsrHandler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx);

    /* @brief Task to calculate the speed of the encoder */
    static void _speedTask(void *pvParameter);
};
