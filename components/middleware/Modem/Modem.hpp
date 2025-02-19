/**
 * @file Modem.h
 * @brief Modem class definition
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "cxx_include/esp_modem_api.hpp"
#include "esp_modem_c_api_types.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"

class Modem
{
private:

public:
    Modem() {}
    ~Modem() {}

    static void init(uart_port_t port, int txPin, int rxPin, gpio_num_t powerPin);

    void connect(const char *apn, const char *sim_pin);
    void disconnect(void);
};