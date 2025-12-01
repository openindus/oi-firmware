/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file AnalogOutputs.cpp
 * @brief
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "AnalogOutputs.h"

static const char TAG[] = "AnalogOutputs";

uint8_t AnalogOutputs::_nb = 0;
AnalogOutput_Mode_t* AnalogOutputs::_modes = NULL;
ad5413_device_t** AnalogOutputs::_devices = NULL;
bool* AnalogOutputs::_devicesInitialized = NULL;

int AnalogOutputs::init(uint8_t nb, ad5413_config_t* configs)
{
    ESP_LOGI(TAG, "Initialize Analog Outputs");
    int err = 0;

    if (nb > AOUT_MAX) {
        ESP_LOGE(TAG, "Invalid number of Analog Outputs");
        return -1;
    } else {
        _nb = nb;
    }

    _modes = (AnalogOutput_Mode_t*) calloc(_nb, sizeof(AnalogOutput_Mode_t));
    _devices = (ad5413_device_t**) malloc(_nb * sizeof(ad5413_device_t*));
    _devicesInitialized = (bool*) calloc(_nb, sizeof(bool));

    if (_devices == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for devices instance");
        return -1;
    }

    if (configs == NULL) {
        ESP_LOGE(TAG, "No configuration");
        return -1;
    }

    for (size_t i = 0; i < _nb; i++) {
        /* Initialize device */
        err |= ad5413_init(&_devices[i], &configs[i]);

        if (err == -1) {
            _devicesInitialized[i] = false;
            ESP_LOGE(TAG, "Failed to Initialize device %d", i);
            return -1;
        }
        _devicesInitialized[i] = true;
    }

    /* Perform commands to start the device */
    for (size_t i = 0; i < _nb; i++) {
        if (_devices[i] == NULL) {
            ESP_LOGE(TAG, "No device");
            return -1;
        }

        err |= ad5413_soft_reset(_devices[i]);
        err |= ad5413_calib_mem_refresh(_devices[i]);
        err |= ad5413_clear_dig_diag_flag(_devices[i], DIG_DIAG_RESET_OCCURRED);
        err |= ad5413_internal_buffers_en(_devices[i], 1);
        err |= ad5413_set_output_range(_devices[i], OUTPUT_RANGE_M10V5_10V5);
        _modes[i] = AOUT_MODE_M10V5_10V5;
        err |= analogWrite((AnalogOutput_Num_t)i, 0.0);
        err |= ad5413_soft_ldac_cmd(_devices[i]);
        err |= ad5413_dac_out_en(_devices[i], 1);
        err |= analogWrite((AnalogOutput_Num_t)i, 0.0);
    }

    _registerCLI();

    return err;
}

int AnalogOutputs::analogOutputMode(AnalogOutput_Num_t num, AnalogOutput_Mode_t mode)
{
    int ret = 0;

    if (num >= _nb) {
        ESP_LOGE(TAG, "Invalid Analog output num");
        return -1;
    }
    if (_devices[num] == NULL) {
        ESP_LOGE(TAG, "No device");
        return -1;
    }

    _modes[num] = mode;

    /* Set mode sequence */
    ret |= analogWrite((AnalogOutput_Num_t)num, 0.0);
    ret |= ad5413_dac_out_en(_devices[num], 0);
    ret |= ad5413_soft_ldac_cmd(_devices[num]);
    ret |= ad5413_internal_buffers_en(_devices[num], 1);
    if (_modes[num] == AOUT_MODE_M10V5_10V5) {
        ret |= ad5413_set_output_range(_devices[num], OUTPUT_RANGE_M10V5_10V5);
    } else if (_modes[num] == AOUT_MODE_0mA_20mA) {
        ret |= ad5413_set_output_range(_devices[num], OUTPUT_RANGE_0mA_20mA);
    } else {
        ESP_LOGE(TAG, "Undefined mode");
        ret = -1;
    }    
    ret |= ad5413_dac_out_en(_devices[num], 1);
    ret |= analogWrite((AnalogOutput_Num_t)num, 0.0);   //Without this, the output is not ensure to be at 0V.

    if (ret == -1) {
        ESP_LOGE(TAG, "Failed to set mode");
        return -1;
    }

    return 0;
}

int AnalogOutputs::analogWrite(AnalogOutput_Num_t num, float value)
{
    int ret = 0;
    uint16_t data = 0;

    if (num >= _nb) {
        ESP_LOGE(TAG, "Invalid Analog output num");
        return -1;
    }
    if (_devices[num] == NULL) {
        ESP_LOGE(TAG, "No device");
        return -1;
    }

    if (_modes[num] == AOUT_MODE_M10V5_10V5) {
        if ((value < -10.5) || (value > 10.5)) {
            ESP_LOGE(TAG, "Value out of range: [-10.5;+10.5V]");
            return -1;
        }
        data = (uint16_t)(((value + 10.5) * (0X3FFF / 21)) - 10.5);
    } else if (_modes[num] == AOUT_MODE_0mA_20mA) {
        if ((value < 0) || (value > 20)) {
            ESP_LOGE(TAG, "Value out of range: [0;20mA]");
            return -1;
        }
        data = (uint16_t)((value) * (0X3FFF / 24));
    } else {
        ESP_LOGE(TAG, "Undefined output mode");
        return -1;
    }

    ret |= ad5413_dac_input_write(_devices[num], data);

    if (ret == -1) {
        ESP_LOGE(TAG, "Failed to write");
        return -1;
    }

    return 0;
}