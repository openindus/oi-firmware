#pragma once

#include "sdkconfig.h"
#include "Netif_Private.h"

#ifndef CONFIG_NETIF_E_ETHERNET
    #error "Netif: The ethernet netif is disabled. You need to enable it in menuconfig:\
    (OpenIndus Configuration -> Netif Configuration -> Ethernet Configuration -> Enable Interface)"
#endif
#ifdef CONFIG_NETIF_E_ETHERNET

#ifndef CONFIG_ETH_SPI_ETHERNET_W5500
    #error "Netif: The wiznet w5500 driver must be enabled in menuconfig." \
            "(Component config -> Ethernet -> Support SPI to Ethernet Module -> Use W5500)"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Connect your module to the internet using ethernet
/// @warning The wiznet w5500 driver must be enabled in menuconfig to use this function.
/// (Component config -> Ethernet -> Support SPI to Ethernet Module -> Use W5500)
/// @param connected_callback Called when the link is up (Could be NULL)
/// @param disconnected_callback Called when the link is down (Could be NULL)
/// @param user_data Data that will be passed to the OINetif structure (Could be NULL)
/// @return Netif_t* Pointer to OINetif structure
Netif_t *ethernet_connect(void (*connected_callback)(const Netif_t *OInetif),
                        void (*disconnected_callback)(const Netif_t *OInetif),
                        void *user_data);

#ifdef __cplusplus
}
#endif
#endif