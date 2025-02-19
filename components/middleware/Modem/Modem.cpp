/**
 * @file Modem.cpp
 * @brief Modem class implementation
 * @author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
 * @copyright (c) [2025] OpenIndus, Inc. All rights reserved.
 * @see https://openindus.com
 */

#include "Modem.hpp"
// #include "Modem_SARA_R4.hpp"

static const char *TAG = "Modem";
static EventGroupHandle_t event_group = NULL;
static const int CONNECT_BIT = BIT0;
static const int DISCONNECT_BIT = BIT1;
static const int GOT_DATA_BIT = BIT2;

static void on_ppp_changed(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "PPP state changed event %" PRIu32, event_id);
    if (event_id == NETIF_PPP_ERRORUSER) {
    /* User interrupted event from esp-netif */
    esp_netif_t **p_netif = (esp_netif_t**)event_data;
    ESP_LOGI(TAG, "User interrupted event from netif:%p", *p_netif);
    }
}

static void on_ip_event(void *arg, esp_event_base_t event_base, int32_t event_id, 
    void *event_data)
{
    ESP_LOGD(TAG, "IP event! %" PRIu32, event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP) {
    esp_netif_dns_info_t dns_info;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    esp_netif_t *netif = event->esp_netif;

    ESP_LOGI(TAG, "Modem Connect to PPP Server");
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
    ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
    ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
    esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
    ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
    esp_netif_get_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
    ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
    ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
    xEventGroupSetBits(event_group, CONNECT_BIT);

    ESP_LOGI(TAG, "GOT ip event!!!");
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
    ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
    xEventGroupSetBits(event_group, DISCONNECT_BIT);
    } else if (event_id == IP_EVENT_GOT_IP6) {
    ESP_LOGI(TAG, "GOT IPv6 event!");

    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
    }
}

static uart_port_t _port;
static int _txPin;
static int _rxPin;

/**
 * @brief 
 * 
 * @param port 
 * @param txPin 
 * @param rxPin 
 * @param powerPin 
 */
void Modem::init(uart_port_t port, int txPin, int rxPin, gpio_num_t powerPin)
{
    _port = port;
    _txPin = txPin;
    _rxPin = rxPin;

    ESP_LOGI(TAG, "Hardware initialization");

    gpio_config_t io_conf;
    io_conf.pin_bit_mask = 1ULL << powerPin;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(powerPin, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(powerPin, 0);
}

/**
 * @brief 
 * 
 * @param apn 
 * @param sim_pin 
 */
void Modem::connect(const char *apn, const char *sim_pin)
{
    /* Init and register system/core components */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL));

    /* Configure the PPP netif */
    esp_err_t err;
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(apn);
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    event_group = xEventGroupCreate();

    /* Configure the DTE */
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    /* setup UART specific configuration based on kconfig options */
    dte_config.uart_config.port_num = _port;
    dte_config.uart_config.tx_io_num = _txPin;
    dte_config.uart_config.rx_io_num = _rxPin;
    dte_config.uart_config.rts_io_num = GPIO_NUM_NC;
    dte_config.uart_config.cts_io_num = GPIO_NUM_NC;

    ESP_LOGI(TAG, "Initializing the module");
    esp_modem_dce_t *dce = esp_modem_new_dev(ESP_MODEM_DCE_CUSTOM, &dte_config, &dce_config, esp_netif);
    assert(dce);

    xEventGroupClearBits(event_group, CONNECT_BIT | GOT_DATA_BIT | DISCONNECT_BIT);

    err = esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_DATA) failed with %d", err);
        return;
    }

    /* Wait for IP address */
    ESP_LOGI(TAG, "Waiting for IP address");
    xEventGroupWaitBits(event_group, CONNECT_BIT | DISCONNECT_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(60000));
    if ((xEventGroupGetBits(event_group) & CONNECT_BIT) != CONNECT_BIT) {
        ESP_LOGW(TAG, "Modem not connected, switching back to the command mode");
        err = esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_COMMAND) failed with %d", err);
            return;
        }
        ESP_LOGI(TAG, "Command mode restored");
        return;
    }
}

/**
 * @brief 
 * 
 */
void Modem::disconnect(void)
{
    ESP_LOGI(TAG, "Disconnecting from the modem...");
}