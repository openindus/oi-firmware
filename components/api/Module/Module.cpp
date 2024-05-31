/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file Module.cpp
 * @brief Generic functions for Module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "Module.h"

static const char TAG[] = "Module";

uint16_t ModuleStandalone::_type = 0;

int ModuleStandalone::init(uint16_t type)
{
    int err = 0;

    _type = type;

    /* Init GPIO service */
    err |= gpio_install_isr_service(0);

    /* Initialize NVS */ 
    esp_err_t ret = nvs_flash_init_partition(NVS_DEFAULT_PART_NAME);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    /* GPIO19 and GPIO20 are USB interface, force reset/config before setting them as GPIO */
    err |= gpio_reset_pin(GPIO_NUM_19);
    err |= gpio_reset_pin(GPIO_NUM_20);

    /* Board */
    Board::init();

    /* LED */
    ESP_LOGI(TAG, "Init LED");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    return err;
}