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

    /* Board */
    ESP_LOGI(TAG, "Board init.");
    err |= Board::init();

    /* LED */
    ESP_LOGI(TAG, "LED init.");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    return err;
}