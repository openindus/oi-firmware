/**
 * @file Discrete.h
 * @brief Discrete
 * @author Georges de Massol
 * @copyright (c) [2024] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "RelayHP.h"

#if defined(OI_RELAY_HP)

static const char RELAY_HP_TAG[] = "RelayHP";

static const gpio_num_t _relayGpio[] = {RELAY_HP_PHASE_CMD4, RELAY_HP_PHASE_CMD3,
                                        RELAY_HP_PHASE_CMD2, RELAY_HP_PHASE_CMD1};

int RelayHPStandalone::init(void)
{
    int err = 0;

    ESP_LOGI(RELAY_HP_TAG, "RelayHPStandalone::init");

    err |= Module::init(TYPE_OI_RELAY_HP);
    err |= Relays::init(_relayGpio, sizeof(_relayGpio) / sizeof(gpio_num_t));

    return err;
}

#if defined(MODULE_SLAVE)

int RelayHPSlave::init(void)
{
    int err = 0;

    err |= RelayHPStandalone::init();
    err |= RelayCmdHandler::init();

    ESP_LOGI(RELAY_HP_TAG, "RelayHPSlave::init");

    return err;
}
#endif

#endif