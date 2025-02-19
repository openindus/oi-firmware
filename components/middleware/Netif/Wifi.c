#include "sdkconfig.h"

#ifdef CONFIG_NETIF_E_WIFI
#include "esp_log.h"
#include "string.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "Wifi_Private.h"
#include "Mac_Private.h"
#include "Event_Private.h"
#include "Netif_Wifi.h"

static bool is_reconnecting = false;

static void wifi_reconnect(void *OIW)
{
    Netif_t *OIWifi = (Netif_t *) OIW;
    int try = 0;
    is_reconnecting = true;

    while ((esp_wifi_connect() != ESP_OK || OIWifi->got_ip == false) && try < WIFI_MAX_RETRY_TASK_RESET) {
        ESP_LOGI(NetifTAG, "Wifi: trying to reconnect...");
        vTaskDelay(WIFI_RETRY_DELAY * 1000);
        if (OIWifi->got_ip == false || OIWifi->is_connected == false) {
            esp_wifi_disconnect();
            ESP_LOGW(NetifTAG, "Wifi: Unable to connect to AP, make sure the SSID and password are correct");
        }
        try++;
        vTaskDelay(WIFI_RETRY_DELAY * 500);
    }
    is_reconnecting = false;
    vTaskDelete(NULL);
}

void wifi_handler(void *event_handler_args, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    Netif_t *OIWifi = (Netif_t *) event_handler_args;
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    switch (event_id) {
    case IP_EVENT_STA_GOT_IP :
        ESP_LOGI(NetifTAG, "Wifi Connected");
        ESP_LOGI(NetifTAG, "Wifi IP:" IPSTR, IP2STR(&ip_info->ip));
        ESP_LOGI(NetifTAG, "Wifi Mask:" IPSTR, IP2STR(&ip_info->netmask));
        ESP_LOGI(NetifTAG, "Wifi Gateway:" IPSTR, IP2STR(&ip_info->gw));
        OIWifi->is_connected = true;
        OIWifi->got_ip = true;
        for (unsigned int i = 0; i != 4; i++)
            OIWifi->ip[i] = esp_ip4_addr_get_byte(&ip_info->ip, i);
        for (unsigned int i = 0; i != 4; i++)
            OIWifi->mask[i] = esp_ip4_addr_get_byte(&ip_info->netmask, i);
        for (unsigned int i = 0; i != 4; i++)
            OIWifi->gateway[i] = esp_ip4_addr_get_byte(&ip_info->gw, i);
        Netif_task_Connected(OIWifi);
        break;

    case IP_EVENT_STA_LOST_IP:
        ESP_LOGI(NetifTAG, "Lost IP");
        OIWifi->got_ip = false;
        for (unsigned int i = 0; i != 4; i++) {
            OIWifi->ip[i] = 0;
            OIWifi->mask[i] = 0;
            OIWifi->gateway[i] = 0;
        }
        Netif_task_Disconnected(OIWifi);
        if (is_reconnecting == false)
            xTaskCreate(wifi_reconnect, "Wifi Reconnect", 8192, OIWifi, 20, NULL);
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        OIWifi->is_connected = false;
        OIWifi->got_ip = false;
        Netif_task_Disconnected(OIWifi);
        if (is_reconnecting == false)
            xTaskCreate(wifi_reconnect, "Wifi Reconnect", 8192, OIWifi, 20, NULL);
        break;

    case WIFI_EVENT_STA_AUTHMODE_CHANGE:
        if (is_reconnecting == false)
            xTaskCreate(wifi_reconnect, "Wifi Reconnect", 8192, OIWifi, 20, NULL);
        break;

    default:
        break;
    }
}

Netif_t *wifi_connect(const char *SSID, const char *Password,
                        void (*connected_callback)(const Netif_t *OInetif),
                        void (*disconnected_callback)(const Netif_t *OInetif),
                        void *user_data)
{
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    Netif_t *OInetif = malloc(sizeof(Netif_t));
    wifi_config_t wifi_config;
    esp_err_t err;

    memset(OInetif, 0, sizeof(Netif_t));
    OInetif->eventConnected = connected_callback;
    OInetif->eventDisconnected = disconnected_callback;
    OInetif->user_data = user_data;

    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(&wifi_config.sta.ssid, SSID, strlen(SSID));
    memcpy(&wifi_config.sta.password, Password, strlen(Password));

    nvs_flash_init();
    ESP_ERROR_CHECK(esp_netif_init());
    err = esp_event_loop_create_default();
    if (err != ESP_OK)
        ESP_LOGW(NetifTAG, "Wifi: An error occured : %s", esp_err_to_name(err));
    esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler, OInetif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_handler, OInetif));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());
    xTaskCreate(wifi_reconnect, "Wifi Reconnect", 8192, OInetif, 20, NULL);
    return (OInetif);
}
#endif
