#pragma once

#include "sdkconfig.h"
#include "Netif_Private.h"

#ifndef CONFIG_NETIF_E_4G
    #error "Netif: The 4G netif is disabled. You need to enable it in menuconfig:\
    (OpenIndus Configuration -> Netif Configuration -> 4G Configuration -> Enable Interface)"
#endif
#ifdef CONFIG_NETIF_E_4G

#if CONFIG_LWIP_PPP_SUPPORT != 1
    #error "OINetif: PPP Support must be enabled in menuconfig (Component config -> LWIP -> Enable PPP support)"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Connect your module to the internet using 4g
/// @warning PPP Support must be enabled in menuconfig to use this function.
/// (Component config -> LWIP -> Enable PPP support)
/// @param APN (Access Point Name) APN of your mobile operator
/// @param SIM_PIN Pin code of your SIM card
/// @param connected_callback Called when the modem is connected (Could be NULL)
/// @param disconnected_callback Called when the modem is disconnected (Could be NULL)
/// @param user_data Data that will be passed to the OINetif structure (Could be NULL)
/// @return Netif_t* Pointer to OINetif structure
Netif_t *lte_connect(const char *APN, const char *SIM_PIN,
                    void (*connected_callback)(const Netif_t *OInetif),
                    void (*disconnected_callback)(const Netif_t *OInetif),
                    void *user_data);

#ifdef __cplusplus
}
#endif
#endif