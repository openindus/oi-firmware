#pragma once

#define NetifTAG "OINetif"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Netif_s Netif_t;

/// @brief The Netif structure allows you to monitor the current status of
/// your Wifi/4G/Ethernet connection.
struct Netif_s {
    /// @brief Check is connected
    bool is_connected;
    /// @brief Device has IP
    bool got_ip;
    /// @brief Raw IP
    unsigned char ip[4];
    /// @brief Raw subnet mask
    unsigned char mask[4];
    /// @brief Raw gateway
    unsigned char gateway[4];
    /// @brief User data pointer
    void *user_data;
    /// @brief Connected callback
    void (*eventConnected)(const Netif_t *eth_handler);
    /// @brief Disconnected callback
    void (*eventDisconnected)(const Netif_t *eth_handler);
} typedef Netif_t;

#ifdef __cplusplus
}
#endif
