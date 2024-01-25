#include "sdkconfig.h"

#ifdef CONFIG_NETIF_E_ETHERNET
#include "string.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "../src/w5500.h"
#include "esp_check.h"

#include "Ethernet_Private.h"
#include "Mac_Private.h"
#include "Event_Private.h"
#include "Netif_Ethernet.h"

#ifdef CONFIG_ETH_SPI_ETHERNET_W5500

/// @brief Handles ethernet IP events
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;
    Netif_t *OIEth = (Netif_t *) arg;

    switch (event_id) {
    case IP_EVENT_ETH_GOT_IP:
        ESP_LOGI(NetifTAG, "Ethernet Got IP Address");
        ESP_LOGI(NetifTAG, "ETH IP:" IPSTR, IP2STR(&ip_info->ip));
        ESP_LOGI(NetifTAG, "ETH Mask:" IPSTR, IP2STR(&ip_info->netmask));
        ESP_LOGI(NetifTAG, "ETH Gateway:" IPSTR, IP2STR(&ip_info->gw));
        OIEth->got_ip = true;
        for (unsigned int i = 0; i != 4; i++)
            OIEth->ip[i] = esp_ip4_addr_get_byte(&ip_info->ip, i);
        for (unsigned int i = 0; i != 4; i++)
            OIEth->mask[i] = esp_ip4_addr_get_byte(&ip_info->netmask, i);
        for (unsigned int i = 0; i != 4; i++)
            OIEth->gateway[i] = esp_ip4_addr_get_byte(&ip_info->gw, i);
        Netif_task_Connected(OIEth);
        break;
    case IP_EVENT_ETH_LOST_IP:
        ESP_LOGW(NetifTAG, "Ethernet Lost IP");
        OIEth->got_ip = false;
        for (unsigned int i = 0; i != 4; i++) {
            OIEth->ip[i] = 0;
            OIEth->mask[i] = 0;
            OIEth->gateway[i] = 0;
        }
        Netif_task_Disconnected(OIEth);
        break;
    default:
        break;
    }
}

/// @brief Handles ethernet events
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    Netif_t *OIEth = (Netif_t *) arg;
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(NetifTAG, "Ethernet Link Up");
        OIEth->is_connected = true;
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        Netif_task_Connected(OIEth);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        OIEth->is_connected = false;
        OIEth->got_ip = false;
        ESP_LOGW(NetifTAG, "Ethernet Link Down");
        Netif_task_Disconnected(OIEth);
        break;
    default:
        break;
    }
}

Netif_t *ethernet_connect(void (*connected_callback)(const Netif_t *OInetif),
                        void (*disconnected_callback)(const Netif_t *OInetif),
                        void *user_data)
{
    esp_err_t err = 0;
    spi_device_handle_t spi_handle = NULL;
    spi_device_interface_config_t spi_devcfg;
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = NULL;
    esp_eth_phy_t *phy = NULL;
    esp_eth_handle_t eth_handle = NULL;
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = NULL;
    Netif_t *OInetif = malloc(sizeof(Netif_t));

    memset(OInetif, 0, sizeof(Netif_t));
    OInetif->eventConnected = connected_callback;
    OInetif->eventDisconnected = disconnected_callback;
    OInetif->user_data = user_data;

    //Create default event loop
    err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        ESP_LOGW(NetifTAG, "Ethernet: An error occured : %s", esp_err_to_name(err));
    }

    //MAC Config
    mac_config.sw_reset_timeout_ms = 1000;

    //PHY Config
    phy_config.phy_addr = -1;
    phy_config.reset_gpio_num = -1;

    //SPI Config
    memset(&spi_devcfg, 0, sizeof(spi_device_interface_config_t));
    spi_devcfg.command_bits = ETH_SPI_COMMAND_BITS;
    spi_devcfg.address_bits = ETH_SPI_ADDRESS_BITS;
    spi_devcfg.mode = 0;
    spi_devcfg.clock_speed_hz = ETH_SPI_CLOCK_SPEED * 1000 * 1000;
    spi_devcfg.spics_io_num = ETH_SPI_CS_PIN;
    spi_devcfg.queue_size = 20;
    spi_devcfg.pre_cb = NULL;
    spi_devcfg.post_cb = NULL;
    ESP_ERROR_CHECK(spi_bus_add_device(ETH_SPI_HOST_ID, &spi_devcfg, &spi_handle));

    //Ethernet w5500 module config
    eth_w5500_config_t w5500_conf = ETH_W5500_DEFAULT_CONFIG(spi_handle);
    w5500_conf.int_gpio_num = ETH_INTERRUPT_PIN;

    mac = esp_eth_mac_new_w5500(&w5500_conf, &mac_config);
    phy = esp_eth_phy_new_w5500(&phy_config);

    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    //Installing the Ethernet driver
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));

    //Set MAC Address
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, (void *) getMacAddr().local_mac));

    //Initialize network interface
    ESP_ERROR_CHECK(esp_netif_init());
    eth_netif = esp_netif_new(&netif_cfg);

    //Registering Ethernet events
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, OInetif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID , &ip_event_handler, OInetif));

    //Attach Ethernet driver to TCP/IP stack and start driver
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    return (OInetif);
}
#endif
#endif