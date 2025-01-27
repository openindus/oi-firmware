/**
 * @file AnalogOutputs.h
 * @brief Analog Outputs
 * @author
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include "ad5413.h"
#include "freertos/task.h"
#include <string.h>

/**
 * @brief Analog Outputs Number
 *
 */
typedef enum { 
    AOUT_1 = 0, 
    AOUT_2 = 1, 
    AOUT_MAX 
} AnalogOutput_Num_t;

/**
 * @brief Analog Outputs Modes
 *
 */
typedef enum {
    AOUT_MODE_M10V5_10V5 = 0,
    AOUT_MODE_0mA_20mA   = 1,
    AOUT_MODE_UNDEFINED
} AnalogOutput_Mode_t;

class AnalogOutputs
{
public:
    /**
     * @brief Initialize Analog Outputs
     *
     * @param nb Number of Analog Outputs
     * @param configs Analog Outputs configurations
     * @return 0 if success, -1 if error
     **/
    static int init(uint8_t nb, ad5413_config_t *configs);

    /**
     * @brief start the devices to enable Outputs
     *
     * @return 0 if success, -1 if error
     **/
    static int start(void);

    /**
     * @brief Set the mode of the specified Analog Output
     *
     * @param num Analog Output number
     * @param mode Analog Output mode
     * @return 0 if success, -1 if error
     **/
    static int analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode);

    /**
     * @brief Set the specified Analog Output to desired voltage value
     *
     * @param num Analog Output number
     * @param value Desired voltage
     * @return 0 if success, -1 if error
     */
    static int analogWrite(AnalogOutput_Num_t num, float value);

    template <typename T1, typename T2>
    static inline int analogWrite(T1 num, T2 value) {
        return analogWrite((AnalogOutput_Num_t)num, (float)value);
    }

private:
    static uint8_t _nb;
    static AnalogOutput_Mode_t *_modes;
    static ad5413_device_t **_devices;
    static bool *_devicesInitialized;
};