/**
 * @file ads114s0x.c
 * @brief ADS114S0x ADC driver
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "ads114s0x.h"

static const char TAG[] = "ads114s0x";

/**
 * @brief Initialize the device
 * 
 * @param dev Device instance
 * @param conf Device config
 * @return int 
 */
int ads114s0x_init(ads114s0x_device_t** dev, ads114s0x_config_t* conf)
{
    ads114s0x_device_t* device = malloc(sizeof(ads114s0x_device_t));
    if (dev == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for the device instance");
        goto error;
    }

    /** @todo: init SPI and GPIOs */

    *dev = device;
    return 0;

error:
    free(device);
    return -1;
}