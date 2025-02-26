/**
 * @file Modem.cpp
 * @brief Modem class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Modem.hpp"

using namespace esp_modem;

static const char *TAG = "Modem";
static std::unique_ptr<DCE> dce;

uart_port_t Modem::_port;
int Modem::_txPin = -1;
int Modem::_rxPin = -1;
gpio_num_t Modem::_powerPin = GPIO_NUM_NC;

/**
 * @brief Set the hardware configuration
 * 
 * @param port 
 * @param txPin 
 * @param rxPin 
 * @param powerPin 
 */
void Modem::setHwConfig(uart_port_t port, int txPin, int rxPin, gpio_num_t powerPin)
{
    _port = port;
    _txPin = txPin;
    _rxPin = rxPin;
    _powerPin = powerPin;

    ESP_LOGI(TAG, "Hardware initialization");

    gpio_config_t io_conf;
    io_conf.pin_bit_mask = 1ULL << powerPin;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

/**
 * @brief Initialize the modem
 * 
 * @param apn 
 * @param sim_pin 
 */
void Modem::begin(const char *apn, const char *sim_pin)
{
    esp_err_t err = ESP_OK;

    ESP_LOGI(TAG, "SARA-R4 module initialization");

    /* Wake up */
    gpio_set_level(_powerPin, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(_powerPin, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(_powerPin, 0);

    /* Initialize netif */
    ESP_ERROR_CHECK(esp_netif_init());
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to create event loop");
        return;
    }

    /* Configure the PPP netif */
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(apn);
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    /* Configure the DTE */
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    dte_config.uart_config.port_num = _port;
    dte_config.uart_config.tx_io_num = _txPin;
    dte_config.uart_config.rx_io_num = _rxPin;
    dte_config.uart_config.rts_io_num = GPIO_NUM_NC;
    dte_config.uart_config.cts_io_num = GPIO_NUM_NC;
    auto uart_dte = create_uart_dte(&dte_config);

    /* Configure the DCE */
    ESP_LOGI(TAG, "DCE configuration");
    dce = create_generic_dce(&dce_config, uart_dte, esp_netif);
    assert(dce != nullptr);

    /* Set command mode */
    auto mode = dce->get_mode();
    if (mode != modem_mode::COMMAND_MODE) {
        if (!dce->set_mode(modem_mode::COMMAND_MODE)) {
            ESP_LOGE(TAG, "Failed to set command mode*");
            return;
        }
    }

    /* Wait for sync */
    for (int attempts = 0; attempts < 10; ++attempts) {
        if (dce->sync() == command_result::OK) {
            break;
        }
        ESP_LOGW(TAG, "Failed to sync, attempt %d", attempts + 1);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    /* Check if PIN needed */
    if (sim_pin != nullptr) {
        bool pin_ok;
        if (dce->read_pin(pin_ok) == command_result::OK) {
            if (dce->set_pin(sim_pin) == command_result::OK && pin_ok == false) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            } else {
                ESP_LOGE(TAG, "Failed to set PIN");
                return;
            }
        } else {
            ESP_LOGE(TAG, "Failed to read PIN");
            return;
        }
    }
}

/**
 * @brief Deinitialize the modem
 * 
 */
void Modem::end(void)
{

}

/**
 * @brief Connect to the network
 * 
 */
void Modem::connect(void)
{
    if (dce != nullptr) {
        _registerEvent(); // Register event handler

        /* Set data mode */
        auto mode = dce->get_mode();
        if (mode != modem_mode::DATA_MODE) {
            if (!dce->set_mode(modem_mode::DATA_MODE)) {
                ESP_LOGE(TAG, "Failed to set data mode");
                return;
            }
        }

        _waitEvent(); // Wait for IP address

    } else {
        ESP_LOGE(TAG, "Modem is not initialized");
        return;
    }
}

/**
 * @brief Disconnect from the network
 * 
 */
void Modem::disconnect(void)
{
    if (dce != nullptr) {
        auto mode = dce->get_mode();
        if (mode != modem_mode::COMMAND_MODE) {
            if (!dce->set_mode(modem_mode::COMMAND_MODE)) {
                ESP_LOGE(TAG, "Failed to set command mode");
                return;
            }
        }
    } else {
        ESP_LOGE(TAG, "Modem is not initialized");
        return;
    }
}

/**
 * @brief Send an SMS
 * 
 * @param number Phone number to send the message to
 * @param message Text message to be sent 
 */
void Modem::sendSMS(const std::string &number, const std::string &message)
{
    if (dce != nullptr) {
        if (dce->sms_txt_mode(true) != command_result::OK ||
            dce->sms_character_set() != command_result::OK) {
            ESP_LOGE(TAG, "Failed to set SMS mode");
            return;
        }
        if (dce->send_sms(number, message) != command_result::OK) {
            ESP_LOGE(TAG, "Failed to send SMS");
            return;
        }
    } else {
        ESP_LOGE(TAG, "Modem is not initialized");
        return;
    }
}

/**
 * @brief Get the signal quality
 * 
 * @param rssi signal strength indication
 * @param ber channel bit error rate
 */
void Modem::getSignalQuality(int &rssi, int &ber)
{
    if (dce != nullptr) {
        if (dce->get_signal_quality(rssi, ber) != command_result::OK) {
            ESP_LOGE(TAG, "Failed to get signal quality");
            return;
        }
    } else {
        ESP_LOGE(TAG, "Modem is not initialized");
        return;
    }
}
