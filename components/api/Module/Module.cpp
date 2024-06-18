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

uint16_t Module::_type = 0;

int Module::init(uint16_t type)
{
    int err = 0;

    /* Board */
    ESP_LOGI(TAG, "Board init.");
    err |= Board::init();

    /* eFuse - Board info */
    uint16_t local_type = getBoardType(); // read it only once to avoid multiple warning
    
    char local_name[16];
    ESP_LOGI(TAG, "Board type       : %u (%s)", local_type, ModuleUtils::typeToName(local_type, local_name));
    ESP_LOGI(TAG, "Serial number    : %d", getSerialNum());
    char hardware_version[4];
    getHardwareVersion(hardware_version);
    ESP_LOGI(TAG, "Hardware version : %.*s", 4, hardware_version);
    char str_date[13];
    time_t t = (time_t) getTimestamp();
    struct tm lt;
    localtime_r(&t, &lt);
    strftime(str_date, sizeof(str_date), "(%d/%m/%Y)", &lt);
    ESP_LOGI(TAG, "Board timestamp  : %lli %s", getTimestamp(), str_date);
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
            char local_name[16];
            ESP_LOGE(TAG, "Incorrect board type detected ! You have program the board as an %s and board is an %s", \
                ModuleUtils::typeToName(type, name), ModuleUtils::typeToName(local_type, local_name));
            err |= -1; // Do not start the code because we are on wrong board
        }
    }

    /* LED */
    ESP_LOGI(TAG, "LED init.");
    Led::install(MODULE_PIN_LED);
    Led::on(LED_BLUE);

    /* CLI */
    err |= CLI_Board::init();
    err |= CLI_Led::init();
    err |= CLI_Bus::init();
    err |= CLI_Controller::init();

#if defined(MODULE_MASTER) || defined(MODULE_SLAVE)

    /* Bus */
    ESP_LOGI(TAG, "Bus init");

    /* Bus RS/CAN */
    err |= BusRS::begin(MODULE_RS_NUM_PORT, MODULE_PIN_RS_UART_TX, MODULE_PIN_RS_UART_RX);
    err |= BusCAN::begin(MODULE_PIN_CAN_TX, MODULE_PIN_CAN_RX);

#if defined(MODULE_MASTER)
    /* Bus IO */
    BusIO::Config_t config = {
        .adcChannelId = MODULE_OI_ID_ADC_CHANNEL,
        .adcWidthId = MODULE_OI_ID_ADC_WIDTH,
        .gpioNumSync = MODULE_PIN_OI_GPIO,
        .gpioModeSync = GPIO_MODE_INPUT_OUTPUT,
        .gpioNumPower = MODULE_PIN_CMD_MOSFET_ALIM,
    };
#elif defined(MODULE_SLAVE)
    /* Bus IO */
    BusIO::Config_t config = {
        .adcChannelId = MODULE_OI_ID_ADC_CHANNEL,
        .adcWidthId = MODULE_OI_ID_ADC_WIDTH,
        .gpioNumSync = MODULE_PIN_OI_GPIO,
        .gpioModeSync = GPIO_MODE_INPUT,
        .gpioNumPower = MODULE_PIN_CMD_MOSFET_ALIM,
    };
#endif
    err |= BusIO::init(&config);
#endif

    return err;
}