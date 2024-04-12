/**
 * Copyright (C) OpenIndus, Inc - All Rights Reserved
 *
 * This file is part of OpenIndus Library.
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * 
 * @file RelayStandalone.cpp
 * @brief Functions for relay module
 *
 * For more information on OpenIndus:
 * @see https://openindus.com
 */

#include "RelayStandalone.h"
#include "RelayPinout.h"

#if defined(OI_RELAY_HP) || defined(OI_RELAY_LP)

static const char RELAY_TAG[] = "Relay";

#if defined(OI_RELAY_LP)
const gpio_num_t relayLpTable[6] = {RELAY_LP_PHASE_CMD1, RELAY_LP_PHASE_CMD2, RELAY_LP_PHASE_CMD3, RELAY_LP_PHASE_CMD4, RELAY_LP_PHASE_CMD5,RELAY_LP_PHASE_CMD6};
#else
const gpio_num_t relayHpTable[4] = {RELAY_HP_PHASE_CMD1, RELAY_HP_PHASE_CMD2, RELAY_HP_PHASE_CMD3, RELAY_HP_PHASE_CMD4};
#endif

void RelayStandalone::init()
{
    ModuleStandalone::init();

    /* Init of ana pins */
    ESP_LOGI(RELAY_TAG, "initializing RELAYS");
    _initRelays();

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<Relay_PIN_CMD_MOSFET);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_set_level(Relay_PIN_CMD_MOSFET, 0);
}

void RelayStandalone::digitalWrite(Relay_t relay, uint8_t level) 
{
    #if defined OI_RELAY_LP
        gpio_set_level(relayLpTable[relay], level);
    #else
        gpio_set_level(relayHpTable[relay], level);
    #endif
}

void RelayStandalone::_initRelays(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = 0;

    #if defined OI_RELAY_LP
    uint8_t relay_nb = RELAY_6;
    #else 
    uint8_t relay_nb = RELAY_4;
    #endif

    for(int i = 0; i <= relay_nb; i++) {
        #if defined OI_RELAY_LP
        io_conf.pin_bit_mask |= (1ULL<<relayLpTable[i]);
        #else
        io_conf.pin_bit_mask |= (1ULL<<relayHpTable[i]);
        #endif
    }

    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

}

#endif