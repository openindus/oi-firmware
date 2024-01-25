#pragma once

#include "sdkconfig.h"
#include "Netif_Private.h"

#ifndef CONFIG_NETIF_E_WIFI
    #error "Netif: The Wifi netif is disabled. You need to enable it in menuconfig:\
    (OpenIndus Configuration -> Netif Configuration -> Wifi Configuration -> Enable Interface)"
#endif
#ifdef CONFIG_NETIF_E_WIFI


#ifdef __cplusplus
extern "C" {
#endif

/// @brief Connect your module to the internet using Wifi
/// @param SSID Name of your Wifi network
/// @param Password Password of your Wifi network
/// @param connected_callback Called when the Wifi is connected (Could be NULL)
/// @param disconnected_callback Called when the Wifi is disconnected (Could be NULL)
/// @param user_data Data that will be passed to the OINetif structure (Could be NULL)
/// @return Netif_t* Pointer to OINetif structure
Netif_t *wifi_connect(const char *SSID, const char *Password,
                        void (*connected_callback)(const Netif_t *OInetif),
                        void (*disconnected_callback)(const Netif_t *OInetif),
                        void *user_data);

#ifdef __cplusplus
}
#endif
#endif