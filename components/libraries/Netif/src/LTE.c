#include "sdkconfig.h"

#ifdef CONFIG_NETIF_E_4G

#include <string.h>
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp32-hal-uart.h"
#include "driver/gpio.h"
#include "esp_modem_api.h"

#include "LTE_Private.h"
#include "Mac_Private.h"
#include "Event_Private.h"
#include "Netif_LTE.h"

#if CONFIG_LWIP_PPP_SUPPORT == 1

/// @brief Handles 4G events
static void on_ppp_changed(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    Netif_t *OILTE = (Netif_t *) arg;

    if (event_id == NETIF_PPP_ERRORCONNECT || event_id == NETIF_PPP_ERRORPEERDEAD) {
        ESP_LOGW(NetifTAG, "Modem Disconnected");
        OILTE->got_ip = false;
        OILTE->is_connected = false;
        for (unsigned int i = 0; i != 4; i++) {
            OILTE->ip[i] = 0;
            OILTE->mask[i] = 0;
            OILTE->gateway[i] = 0;
        }
        if (OILTE->eventDisconnected != NULL)
            OILTE->eventDisconnected(OILTE);
    }
    if (event_id == NETIF_PPP_ERRORPARAM) {
        ESP_LOGW(NetifTAG, "Modem: Invalid parameter");
    }
}

/// @brief Handles 4G IP events
static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    Netif_t *OILTE = (Netif_t *) arg;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;

    if (event_id == IP_EVENT_PPP_GOT_IP) {
        const esp_netif_ip_info_t *ip_info = &event->ip_info;
        ESP_LOGI(NetifTAG, "Modem Connect to PPP Server");
        ESP_LOGI(NetifTAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(NetifTAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(NetifTAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
        OILTE->is_connected = true;
        OILTE->got_ip = true;
        for (unsigned int i = 0; i != 4; i++)
            OILTE->ip[i] = esp_ip4_addr_get_byte(&ip_info->ip, i);
        for (unsigned int i = 0; i != 4; i++)
            OILTE->mask[i] = esp_ip4_addr_get_byte(&ip_info->netmask, i);
        for (unsigned int i = 0; i != 4; i++)
            OILTE->gateway[i] = esp_ip4_addr_get_byte(&ip_info->gw, i);
        Netif_task_Connected(OILTE);
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGW(NetifTAG, "Modem Disconnect from PPP Server");
        OILTE->got_ip = false;
        for (unsigned int i = 0; i != 4; i++) {
            OILTE->ip[i] = 0;
            OILTE->mask[i] = 0;
            OILTE->gateway[i] = 0;
        }
        Netif_task_Disconnected(OILTE);
    }
}

Netif_t *lte_connect(const char *APN, const char *SIM_PIN,
                    void (*connected_callback)(const Netif_t *OInetif),
                    void (*disconnected_callback)(const Netif_t *OInetif),
                    void *user_data)
{
    esp_err_t err = 0;
    Netif_t *OInetif = malloc(sizeof(Netif_t));
    int rssi, ber = 0;
    bool pin_ok = false;

    memset(OInetif, 0, sizeof(Netif_t));
    OInetif->eventConnected = connected_callback;
    OInetif->eventDisconnected = disconnected_callback;
    OInetif->user_data = user_data;

    /* Init and register system/core components */
    ESP_ERROR_CHECK(esp_netif_init());
    err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        ESP_LOGW(NetifTAG, "LTE: An error occured : %s", esp_err_to_name(err));
    }
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, OInetif));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, OInetif));

    /* Configure the PPP netif */
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(APN);
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    /* Configure the DTE */
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    dte_config.uart_config.tx_io_num = MODEM_TX_GPIO;
    dte_config.uart_config.rx_io_num = MODEM_RX_GPIO;
    dte_config.uart_config.rts_io_num = MODEM_RTS_GPIO;
    dte_config.uart_config.cts_io_num = MODEM_CTS_GPIO;
    dte_config.uart_config.baud_rate = MODEM_BAUD_RATE;
    dte_config.uart_config.flow_control = ESP_MODEM_FLOW_CONTROL_NONE;
    dte_config.uart_config.port_num = MODEM_UART_PORT_NUM;
    dte_config.task_priority = 5;

    /* Syncing With Modem */
    esp_modem_dce_t *dce = esp_modem_new_dev(MODEM_MODEL, &dte_config, &dce_config, esp_netif);
    while (esp_modem_sync(dce) != ESP_OK) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(NetifTAG, "Syncing with modem...");
        esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
    };

    esp_modem_set_radio_state(dce, 1);
    ESP_ERROR_CHECK(esp_modem_read_pin(dce, &pin_ok));
    if (pin_ok != true)
        if (esp_modem_set_pin(dce, SIM_PIN) != ESP_OK) {
            ESP_LOGW(NetifTAG, "Modem: Failed to unlock SIM, make sure the pin is correct");
            esp_modem_destroy(dce);
            esp_netif_destroy(esp_netif);
            free(OInetif);
            return (NULL);
        }
    #if MODEM_ENABLE_NB_IOT == true
        esp_modem_set_preferred_mode(dce, 3);
    #else
        esp_modem_set_preferred_mode(dce, 1);
    #endif
    esp_modem_get_signal_quality(dce, &rssi, &ber);
    ESP_LOGI(NetifTAG, "Modem Signal quality: rssi=%d, ber=%d", rssi, ber);
    ESP_ERROR_CHECK(esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA));
    return (OInetif);
}
#endif
#endif