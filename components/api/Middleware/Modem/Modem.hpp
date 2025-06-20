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
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dte.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"

class Modem
{
public:
    Modem() {}
    ~Modem() {}

    friend class Core;

    void begin(const char *apn, const char *sim_pin = nullptr);
    void end(void);

    void connect(void);
    void disconnect(void);

    void sendSMS(const std::string &number, const std::string &message);
    void getSignalQuality(int &rssi, int &ber);
    // void getTime(std::string &time);

protected:

    static void setHwConfig(uart_port_t port, int txPin, int rxPin, gpio_num_t powerPin);

private:
    static uart_port_t _port;
    static int _txPin;
    static int _rxPin;
    static gpio_num_t _powerPin;

    static void _registerEvent(void);
    static void _waitEvent(void);
};