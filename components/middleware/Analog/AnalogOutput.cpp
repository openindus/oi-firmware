/**
 * @file AnalogOutput.cpp
 * @brief Analog Output
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "AnalogOutput.h"

static const char TAG[] = "AnalogOutput";

bool AnalogOutputAD5413::_deviceInitialized = false;
ad5413_device_t** AnalogOutputAD5413::_device;

int AnalogOutputAD5413::init(ad5413_config_t* config, int nb)
{
    int ret = 0;

    if (config == NULL) {
        ESP_LOGE(TAG, "No configuration");
        return -1;
    } 

    _device = (ad5413_device_t**)malloc(nb * sizeof(ad5413_device_t*));
    if (_device == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for devices instance");
        return -1;
    }

    for (int i = 0; i < nb; i++) {
        if (config != NULL) {

            ESP_LOGI(TAG, "Initialized device %d", i);

            /* Initialized the device */
            ret |= ad5413_init(&_device[i], &config[i]);

            /* Perform commands to start the device */
            ret |= ad5413_soft_reset(_device[i]);
            ret |= ad5413_calib_mem_refresh(_device[i]);
            ret |= ad5413_clear_dig_diag_flag(_device[i], DIG_DIAG_RESET_OCCURRED);
        }
    }

    if (ret != -1) {
        _deviceInitialized = true;
    } else {
        ESP_LOGE(TAG, "Failed to Initialize devices");
        return -1;
    }
    
    return ret;
}

int AnalogOutputAD5413::setMode(AnalogOutput_Mode_t mode)
{
    int ret = 0;

    if (_device == NULL) {
        ESP_LOGE(TAG, "No device");
        return -1;
    }

    _mode = mode;

    /* Set mode sequence */
    ret |= ad5413_dac_out_en(_device[_num], 0);
    ret |= ad5413_dac_input_write(_device[_num], 0);
    ret |= ad5413_soft_ldac_cmd(_device[_num]);
    ret |= ad5413_internal_buffers_en(_device[_num], 1);
    if (_mode == AOUT_MODE_M10V5_10V5) {
        ret |= ad5758_set_output_range(_device[_num], OUTPUT_RANGE_M10V5_10V5);
    } else if (_mode == AOUT_MODE_0mA_20mA) {
        ret |= ad5758_set_output_range(_device[_num], OUTPUT_RANGE_0mA_20mA);
    } else {
        ESP_LOGE(TAG, "Undefined mode");
        ret = -1;
    }    
    ret |= ad5413_dac_out_en(_device[_num], 1);

    if (ret == -1) {
        ESP_LOGE(TAG, "Failed to set mode");
        return -1;
    }

    return 0;
}

int AnalogOutputAD5413::write(float value)
{
    int ret = 0;
    uint16_t data = 0;

    if (_device[_num] == NULL) {
        ESP_LOGE(TAG, "No device");
        return -1;
    }

    if (_mode == AOUT_MODE_M10V5_10V5) {
        if ((value < -10.5) || (value > 10.5)) {
            ESP_LOGE(TAG, "Value out of range: [-10.5;+10.5V]");
            return -1;
        }
        data = (uint16_t)(((value + 10.5) * (0X3FFF / 21)) - 10.5);
    } else if (_mode == AOUT_MODE_0mA_20mA) {
        if ((value < -10.5) || (value > 10.5)) {
            ESP_LOGE(TAG, "Value out of range: [0;20mA]");
            return -1;
        }
        data = (uint16_t)((value) * (0X3FFF / 20));
    } else {
        ESP_LOGE(TAG, "Undefined output mode");
        return -1;
    }

    ret |= ad5413_dac_input_write(_device[_num], data);
    if (ret == -1) {
        ESP_LOGE(TAG, "Failed to write");
        return -1;
    }

    return 0;
}