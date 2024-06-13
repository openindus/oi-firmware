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

    /* eFuse - Board info */
    uint16_t local_type = getBoardType(); // read it only once to avoid multiple warning
    ESP_LOGI(TAG, "Board type       : %u", local_type);
    ESP_LOGI(TAG, "Serial number    : %d", getSerialNum());
    char hardware_version[4];
    getHardwareVersion(hardware_version);
    ESP_LOGI(TAG, "Hardware version : %.*s", 4, hardware_version);
    ESP_LOGI(TAG, "Board timestamp  : %lli", getTimestamp());
    char software_version[32];
    getSoftwareVersion(software_version);
    ESP_LOGI(TAG, "Software version : %s", software_version);

    if (local_type != type) {
        // Hack because we cannot differentiate OI_CORE and OI_CORELITE for now...
        if (type == TYPE_OI_CORE && local_type == TYPE_OI_CORELITE) {
            ESP_LOGE(TAG, "OICoreLite type checked");
        } else if (local_type == 0){
            ESP_LOGW(TAG, "Cannot check board type");
            _type = type; // Save type because the one in eFuse is wrong
        } else {
            char name[16];
            ESP_LOGE(TAG, "Incorrect board type detected ! You have program the board as an %s and board is an %s", \
                ModuleUtils::typeToName(type, name), ModuleUtils::typeToName(local_type, name));
            err |= -1; // Do not start the code because we are on wrong board
        }
    }

    /* LED */
    ESP_LOGI(TAG, "LED init.");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    /* CLI */
    err |= ModuleCLI::init();

    return err;
}